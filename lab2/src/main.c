// main.c
// Thread de I/O (gera u, lê y, grava sim_out.tsv) + medição de período/jitter (CSV)
// Mantém trocas EXCLUSIVAMENTE via API do módulo (u -> sim; y <- sim).
#define _POSIX_C_SOURCE 200809L


#include "sim_robot.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

// ====== Parâmetros do laboratório (mesmos usados em sim_robot_init) ======
static const double DT_IDEAL = 0.05;   // 50 ms
static const double T_END    = 20.0;   // 20 s

// ====== Helpers de tempo (CLOCK_MONOTONIC) ======
static inline long long ns_from_ts(struct timespec ts) {
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}
static inline struct timespec ts_from_ns(long long ns) {
    struct timespec ts;
    ts.tv_sec  = ns / 1000000000LL;
    ts.tv_nsec = ns % 1000000000LL;
    return ts;
}
static inline long long now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ns_from_ts(ts);
}

// ====== Carga opcional (para “rodada com carga”) =========================
// Uma thread de “carga” que consome CPU continuamente (sem comunicar nada).
// Use com parcimônia: ela cria pressão de CPU para observar jitter.
static volatile int g_run_load = 0;

static void *load_thread_fn(void *arg) {
    (void)arg;
    while (g_run_load) {
        // Busy work ~ 1-2 ms
        volatile double x = 0.0;
        for (int i = 0; i < 200000; ++i) x += i * 0.000001;
        // Curto nanosleep para não travar o sistema
        struct timespec ts = {0, 1000000}; // 1 ms
        nanosleep(&ts, NULL);
    }
    return NULL;
}

// ====== Estrutura para passar opções à thread de I/O =====================
typedef struct {
    const char *periods_csv; // nome do CSV para T(k), J(k)
    const char *tsv_out;     // nome do arquivo de amostras (yx, yy)
} IOArgs;

// ====== Thread de I/O (gera u, espera y, grava arquivos e mede T/J) ======
static void *io_thread_fn(void *arg) {
    IOArgs *args = (IOArgs *)arg;

    // Arquivo com as amostras da simulação (ponto frontal y(t))
    FILE *fp = fopen(args->tsv_out, "w");
    if (!fp) { perror("Erro abrindo sim_out.tsv"); exit(1); }
    // Buffers grandes para reduzir impacto no tempo
    setvbuf(fp, NULL, _IOFBF, 1<<20);
    fprintf(fp, "t(s)\tv(m/s)\tw(rad/s)\tyx(m)\tyy(m)\n");

    // Arquivo novo para períodos e jitter
     FILE *fpP = fopen(args->periods_csv, "w");
    if (!fpP) { perror("Erro abrindo periods.csv"); exit(1); }
    setvbuf(fpP, NULL, _IOFBF, 1<<20);
    fprintf(fpP, "k,t_wall(s),T(s),J(s)\n");

    // Configuração da periodicidade ABSOLUTA
    const long long DT_NS = (long long)(DT_IDEAL * 1e9);
    long long next_wakeup_ns = now_ns() + DT_NS;

    // Medição baseada em wake-ups
    const int WARMUP_DROP = 5; // descartar primeiras N amostras
    int seq = 0;               // seq utilizada para sincronizar com simulação
    int k_meas = 0;            // índice de amostras válidas no CSV de períodos
    double t_k = 0.0;          // tempo lógico do início de cada passo
    long long prev_wake_ns = 0;

    while (t_k < T_END - 1e-12) {
        // 1) Aguarda o instante ideal (TIMER_ABSTIME evita drift)
        struct timespec ts_wakeup = ts_from_ns(next_wakeup_ns);
        // Ignora retornos por sinais (simplificação); para robustez, checar retorno
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts_wakeup, NULL);

        // 2) Marca o instante real de ativação (wake-up)
        long long now_wake_ns = now_ns();

        // 3) Gera u(t_k), publica e espera y(t_{k+1})
        double v, w;
        simrobot_generate_u(t_k, &v, &w);
        simrobot_publish_input(t_k, v, w, seq);

        double t_next, yx, yy, theta;
        simrobot_wait_output(seq, &t_next, &yx, &yy, &theta);

        // 4) Grava amostra principal (modelo)
        fprintf(fp, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\n",
                t_next, v, w, yx, yy);

        // 5) Calcula T(k) e J(k) em cima dos wake-ups
        if (seq == 0) {
            prev_wake_ns = now_wake_ns; // não mede k=0
        } else {
            double T_s = (now_wake_ns - prev_wake_ns) / 1e9;
            double J_s = T_s - DT_IDEAL;

            if (seq > WARMUP_DROP) { // ignora período de aquecimento
                fprintf(fpP, "%d,%.9f,%.9f,%.9f\n",
                        k_meas, now_wake_ns / 1e9, T_s, J_s);
                k_meas++;
            }
            prev_wake_ns = now_wake_ns;
        }

        // 6) Agenda o próximo disparo ideal e avança tempo lógico
        next_wakeup_ns += DT_NS;
        seq++;
        t_k += DT_IDEAL;
    }

    fclose(fpP);
    fclose(fp);
    puts("OK: arquivos gerados.");
    return NULL;
}

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Uso: %s [--load]\n"
        "  --load : inicia uma thread de carga para medir o jitter 'com carga'\n"
        "Sem argumentos: mede 'sem carga'.\n", prog);
}

int main(int argc, char **argv) {
    // ====== Parse simples de argumentos ======
    bool with_load = false;
    if (argc >= 2) {
        if (strcmp(argv[1], "--load") == 0) {
            with_load = true;
        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    // ====== Inicializa parâmetros do laboratório ======
    SimParams params = {
        .dt    = DT_IDEAL,
        .t_end = T_END,
        .D     = 0.30
    };
    simrobot_init(&params);

    // ====== Inicia a thread de simulação ======
    if (simrobot_start() != 0) {
        perror("pthread_create(sim)");
        return 1;
    }

    // ====== [CARGA OPCIONAL] Inicia carga se solicitado ======
    pthread_t th_load;
    if (with_load) {
        g_run_load = 1;
        if (pthread_create(&th_load, NULL, load_thread_fn, NULL) != 0) {
            perror("pthread_create(load)");
            g_run_load = 0;
        }
    }

    // ====== Thread de I/O: define nomes dos arquivos por modo ======
    IOArgs io_args = {
        .periods_csv = with_load ? "periods_com_carga.csv" : "periods_sem_carga.csv",
        .tsv_out     = "sim_out.tsv"
    };

    pthread_t th_io;
    if (pthread_create(&th_io, NULL, io_thread_fn, &io_args) != 0) {
        perror("pthread_create(io)");
        return 1;
    }

    // ====== Aguarda término ======
    pthread_join(th_io, NULL);
    simrobot_join();

    // ====== Finaliza carga se estiver ativa ======
    if (with_load && g_run_load) {
        g_run_load = 0;
        pthread_join(th_load, NULL);
    }

    simrobot_destroy();
    return 0;
}
