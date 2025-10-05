
#include "linearization.h"
#include "monitor.h"
#include "time_utils.h"

void *th_linearization(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_LIN_NS);

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        double th, xc, yc; get_x(&xc,&yc,&th);
        double vx, vy; get_v(&vx, &vy);

        // u = L^{-1}(x) v
        double u1 = cos(th)*vx + sin(th)*vy;
        double u2 = (-sin(th)/R)*vx + (cos(th)/R)*vy;
        set_u(u1, u2);

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_LIN, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_LIN_NS);
    }
    return NULL;
}
