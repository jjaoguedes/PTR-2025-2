
#include "control.h"
#include "monitor.h"
#include "time_utils.h"

void *th_control(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_CTRL_NS);

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        double y1,y2; get_y(&y1,&y2);
        double ymx,dymx, ymy,dymy; get_model_x(&ymx,&dymx); get_model_y(&ymy,&dymy);
        double a1,a2; get_alpha(&a1,&a2);

        double vx = dymx + a1*(ymx - y1);
        double vy = dymy + a2*(ymy - y2);
        set_v(vx, vy);

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_CTRL, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_CTRL_NS);
    }
    return NULL;
}
