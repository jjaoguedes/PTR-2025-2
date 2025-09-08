// main.c
// Thread de I/O (geração de u, leitura de y_f e armazenamento).
// Mantém as trocas EXCLUSIVAMENTE via API do módulo (u -> sim; y <- sim).

// --- Feature test macros ---
#define _POSIX_C_SOURCE 199309L

#include "sim_robot.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Defina 1 se quiser marcar ~50 ms de relógio real entre amostras
#define RUNTIME_SLEEP 0

static void sleep_50ms_if_enabled(void) {
#if RUNTIME_SLEEP
    struct timespec ts = {0, 50 * 1000 * 1000};
    nanosleep(&ts, NULL);
#endif
}

typedef struct {
    // Nada por ora; reservado p/ futuras extensões
    int placeholder;
} IOArgs;

static void *io_thread_fn(void *arg) {
    (void)arg;

    FILE *fp = fopen("sim_out.tsv", "w");
    if (!fp) {
        perror("Erro abrindo sim_out.tsv");
        exit(1);
    }
    fprintf(fp, "t(s)\tv(m/s)\tw(rad/s)\tyx(m)\tyy(m)\n");

    // Recupera parâmetros para saber dt e t_end
    // (Poderíamos ter getters; aqui mantemos em sincronia com o que passamos)
    const double dt    = 0.05;
    const double t_end = 20.0;

    int seq = 0;
    double t_k = 0.0; // tempo no início de cada passo

    while (t_k < t_end - 1e-12) {
        // 1) Gera u(t_k) conforme especificação
        double v, w;
        simrobot_generate_u(t_k, &v, &w);

        // 2) Publica u_k
        simrobot_publish_input(t_k, v, w, seq);

        // 3) Espera y_{k+1} (mesma seq)
        double t_next, yx, yy, theta;
        simrobot_wait_output(seq, &t_next, &yx, &yy, &theta);

        // 4) Grava linha (t_{k+1}, u_k, y_{k+1})
        fprintf(fp, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\n",
                t_next, v, w, yx, yy);

        // Avança passo
        seq++;
        t_k += dt;

        // Opcional: “marcar” 50 ms de relógio real
        sleep_50ms_if_enabled();
    }

    fclose(fp);
    puts("OK: arquivo 'sim_out.tsv' gerado.");
    return NULL;
}

int main(void) {
    // 1) Inicializa parâmetros do laboratório
    SimParams params = {
        .dt    = 0.05,  // 50 ms
        .t_end = 20.0,  // 20 s
        .D     = 0.30   // 30 cm
    };
    simrobot_init(&params);

    // 2) Inicia a thread de simulação (mantém x e integra)
    if (simrobot_start() != 0) {
        perror("pthread_create(sim)");
        return 1;
    }

    // 3) Inicia a thread de I/O (gera u, lê y e armazena)
    pthread_t th_io;
    if (pthread_create(&th_io, NULL, io_thread_fn, NULL) != 0) {
        perror("pthread_create(io)");
        return 1;
    }

    // 4) Aguarda término
    pthread_join(th_io, NULL);
    simrobot_join();

    // 5) (Opcional) destruir recursos
    simrobot_destroy();
    return 0;
}
