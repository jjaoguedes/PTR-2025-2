
#include "time_utils.h"
#include <pthread.h>

static FILE *g_perf = NULL;
static pthread_mutex_t g_perf_mtx = PTHREAD_MUTEX_INITIALIZER;

static FILE *g_traj = NULL;
static pthread_mutex_t g_traj_mtx = PTHREAD_MUTEX_INITIALIZER;

double now_since_t0(const struct timespec *t0) {
    struct timespec nt; clock_gettime(CLOCK_MONOTONIC, &nt);
    return (double)ts_diff_ns(&nt, t0) / 1e9;
}

void perf_open(const char *path) {
    pthread_mutex_lock(&g_perf_mtx);
    if (!g_perf) {
        g_perf = fopen(path, "w");
        if (g_perf) fprintf(g_perf, "t,task,comp_ms,jitter_ms\n");
    }
    pthread_mutex_unlock(&g_perf_mtx);
}
void perf_close(void) {
    pthread_mutex_lock(&g_perf_mtx);
    if (g_perf) { fclose(g_perf); g_perf = NULL; }
    pthread_mutex_unlock(&g_perf_mtx);
}
void perf_log(const char *task, double tsec, double comp_ms, double jitter_ms) {
    pthread_mutex_lock(&g_perf_mtx);
    if (g_perf) fprintf(g_perf, "%.6f,%s,%.3f,%.3f\n", tsec, task, comp_ms, jitter_ms);
    pthread_mutex_unlock(&g_perf_mtx);
}

void traj_open(const char *path) {
    pthread_mutex_lock(&g_traj_mtx);
    if (!g_traj) {
        g_traj = fopen(path, "w");
        if (g_traj) fprintf(g_traj, "t,y1,y2,xref,yref\n");
    }
    pthread_mutex_unlock(&g_traj_mtx);
}
void traj_close(void) {
    pthread_mutex_lock(&g_traj_mtx);
    if (g_traj) { fclose(g_traj); g_traj = NULL; }
    pthread_mutex_unlock(&g_traj_mtx);
}
void traj_log(double tsec, double y1, double y2, double xref, double yref) {
    pthread_mutex_lock(&g_traj_mtx);
    if (g_traj) fprintf(g_traj, "%.6f,%.6f,%.6f,%.6f,%.6f\n", tsec, y1, y2, xref, yref);
    pthread_mutex_unlock(&g_traj_mtx);
}
