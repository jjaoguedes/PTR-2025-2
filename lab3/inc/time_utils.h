
#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include "common.h"

static inline struct timespec ts_add_ns(struct timespec t, long ns) {
    t.tv_nsec += ns;
    while (t.tv_nsec >= 1000000000L) { t.tv_sec++; t.tv_nsec -= 1000000000L; }
    while (t.tv_nsec < 0)            { t.tv_sec--; t.tv_nsec += 1000000000L; }
    return t;
}
static inline long ts_diff_ns(const struct timespec *a, const struct timespec *b) {
    // retorna a-b em nanossegundos
    return (a->tv_sec - b->tv_sec) * 1000000000LL + (a->tv_nsec - b->tv_nsec);
}
double now_since_t0(const struct timespec *t0);     // segundos em double

// logging de desempenho thread-safe
void perf_log(const char *task, double tsec, double comp_ms, double jitter_ms);
void perf_open(const char *path);
void perf_close(void);

// logging de trajetória (y, ref)
void traj_open(const char *path);
void traj_close(void);
void traj_log(double tsec, double y1, double y2, double xref, double yref);

#endif
