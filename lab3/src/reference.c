
#include "reference.h"
#include "monitor.h"
#include "time_utils.h"

void *th_reference(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_REF_NS);

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        double t = now_since_t0(&t0);
        set_ref(xref_func(t), yref_func(t));

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_REF, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_REF_NS);
    }
    return NULL;
}
