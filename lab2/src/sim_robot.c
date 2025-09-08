// sim_robot.c
// Modelo do robô diferencial, integrador RK4 e thread de simulação.
// A thread de simulação mantém o estado x e só se comunica por u(t) e y_f(t).

// --- Feature test macros ---
#define _POSIX_C_SOURCE 199309L

// --- Definições portáveis para constantes matemáticas (caso não existam) ---
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

#include "sim_robot.h"

#include <math.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

// ----------------- Armazenamento de parâmetros -----------------
static SimParams g_params = {0};

// ----------------- Mailboxes internas (protegidas) -----------------
typedef struct {
    double v, w;     // entrada
    int    seq;      // sequência da entrada
    double t;        // tempo t_k (início do passo)
} InputMB;

typedef struct {
    double yx, yy;   // saída (ponto frontal)
    double theta;    // orientação (opcional)
    int    seq;      // sequência correspondente
    double t;        // tempo t_{k+1} (fim do passo)
} OutputMB;

static InputMB  g_input;
static OutputMB g_output;

// Sinalização
static pthread_mutex_t g_mtx      = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_cond_in  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  g_cond_out = PTHREAD_COND_INITIALIZER;

static int g_input_seq  = -1;  // última entrada publicada
static int g_output_seq = -1;  // última saída publicada

// Thread de simulação
static pthread_t g_sim_thread;
static int       g_sim_started = 0;

// ----------------- Dinâmica contínua e utilitários -----------------
static inline void f_dyn(const double x[3], double v, double w, double dx[3]) {
    // Atenção: seguindo sua formulação (sin, cos):
    // x1_dot = v * sin(theta)
    // x2_dot = v * cos(theta)
    // x3_dot = w
    dx[0] = v * sin(x[2]);
    dx[1] = v * cos(x[2]);
    dx[2] = w;
}

static void rk4_step(double x[3], double v, double w, double dt) {
    double k1[3], k2[3], k3[3], k4[3], xtmp[3];

    f_dyn(x, v, w, k1);
    for (int i = 0; i < 3; ++i) xtmp[i] = x[i] + 0.5 * dt * k1[i];

    f_dyn(xtmp, v, w, k2);
    for (int i = 0; i < 3; ++i) xtmp[i] = x[i] + 0.5 * dt * k2[i];

    f_dyn(xtmp, v, w, k3);
    for (int i = 0; i < 3; ++i) xtmp[i] = x[i] + dt * k3[i];

    f_dyn(xtmp, v, w, k4);

    for (int i = 0; i < 3; ++i) {
        x[i] += (dt / 6.0) * (k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i]);
    }
}

static inline void front_point(const double x[3], double D, double *yx, double *yy) {
    *yx = x[0] + 0.5 * D * cos(x[2]);
    *yy = x[1] + 0.5 * D * sin(x[2]);
}

// ----------------- API pública -----------------
void simrobot_init(const SimParams *params) {
    if (params) {
        g_params = *params;
    } else {
        g_params.dt = 0.05;
        g_params.t_end = 20.0;
        g_params.D = 0.30;
    }
    // Zera caixas de correio e sequências
    pthread_mutex_lock(&g_mtx);
    memset(&g_input, 0, sizeof(g_input));
    memset(&g_output, 0, sizeof(g_output));
    g_input_seq  = -1;
    g_output_seq = -1;
    pthread_mutex_unlock(&g_mtx);
}

void simrobot_destroy(void) {
    // Opcional: destruir mutex/cond se desejar reutilizar processo várias vezes
    pthread_mutex_destroy(&g_mtx);
    pthread_cond_destroy(&g_cond_in);
    pthread_cond_destroy(&g_cond_out);
}

void simrobot_publish_input(double t_k, double v, double w, int seq) {
    pthread_mutex_lock(&g_mtx);
    g_input.t   = t_k;
    g_input.v   = v;
    g_input.w   = w;
    g_input.seq = seq;
    g_input_seq = seq;
    pthread_cond_signal(&g_cond_in);
    pthread_mutex_unlock(&g_mtx);
}

void simrobot_wait_output(int expected_seq, double *t_next, double *yx, double *yy, double *theta) {
    pthread_mutex_lock(&g_mtx);
    while (g_output_seq < expected_seq) {
        pthread_cond_wait(&g_cond_out, &g_mtx);
    }
    if (t_next) *t_next = g_output.t;
    if (yx)     *yx     = g_output.yx;
    if (yy)     *yy     = g_output.yy;
    if (theta)  *theta  = g_output.theta;
    pthread_mutex_unlock(&g_mtx);
}

void simrobot_generate_u(double t, double *v, double *w) {
    if (t < 0.0) { 
        if(v) *v = 0.0; 
        if(w) *w = 0.0; 
        return; 
    }
    if (t < 10.0) { 
        if(v) *v = 1.0; 
        if(w) *w = 0.2 * M_PI; 
        return; 
    }
    if(v) *v = 1.0; 
    if(w) *w = -0.2 * M_PI;
}

// ----------------- Thread de simulação (interna) -----------------
static void *sim_thread_fn(void *arg) {
    (void)arg;
    double x[3] = {0.0, 0.0, 0.0};  // x(0)=0
    double t_cur = 0.0;
    int last_consumed_in = -1;

    const double dt = g_params.dt;
    const double D  = g_params.D;
    const double T_end = g_params.t_end;

    while (1) {
        // 1) Espera a próxima entrada (seq > last_consumed_in)
        double v, w;
        int my_seq;

        pthread_mutex_lock(&g_mtx);
        while (g_input_seq <= last_consumed_in) {
            pthread_cond_wait(&g_cond_in, &g_mtx);
        }
        v = g_input.v;
        w = g_input.w;
        my_seq = g_input.seq;
        pthread_mutex_unlock(&g_mtx);

        // 2) Integra 1 passo
        rk4_step(x, v, w, dt);
        t_cur += dt;

        // 3) Calcula y_f e publica
        double yx, yy;
        front_point(x, D, &yx, &yy);

        pthread_mutex_lock(&g_mtx);
        g_output.yx   = yx;
        g_output.yy   = yy;
        g_output.theta= x[2];
        g_output.t    = t_cur;
        g_output.seq  = my_seq;
        g_output_seq  = my_seq;
        pthread_cond_signal(&g_cond_out);
        pthread_mutex_unlock(&g_mtx);

        last_consumed_in = my_seq;

        if (t_cur >= T_end - 1e-12) break;
    }
    return NULL;
}

int simrobot_start(void) {
    if (g_sim_started) return 0;
    int rc = pthread_create(&g_sim_thread, NULL, sim_thread_fn, NULL);
    if (rc == 0) g_sim_started = 1;
    return rc;
}

void simrobot_join(void) {
    if (!g_sim_started) return;
    pthread_join(g_sim_thread, NULL);
    g_sim_started = 0;
}
