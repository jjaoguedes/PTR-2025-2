
#include "ref_model.h"
#include "monitor.h"
#include "time_utils.h"

void *th_model_x(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_MODEL_NS);
    double ymx = 0.0;

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        double xr, yr; get_ref(&xr,&yr);
        double a1, a2; get_alpha(&a1,&a2);
        double dymx = a1 * (xr - ymx);
        ymx += (PERIOD_MODEL_NS / 1e9) * dymx;
        set_model_x(ymx, dymx);

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_MX, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_MODEL_NS);
    }
    return NULL;
}

void *th_model_y(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_MODEL_NS);
    double ymy = 0.0;

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        double xr, yr; get_ref(&xr,&yr);
        double a1, a2; get_alpha(&a1,&a2);
        double dymy = a2 * (yr - ymy);
        ymy += (PERIOD_MODEL_NS / 1e9) * dymy;
        set_model_y(ymy, dymy);

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_MY, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_MODEL_NS);
    }
    return NULL;
}
