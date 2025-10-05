
#include "plant.h"
#include "monitor.h"
#include "time_utils.h"

void *th_robot(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_ROBOT_NS);

    double xc=0, yc=0, th=0; set_x(xc,yc,th);
    double y1 = xc + R*cos(th), y2 = yc + R*sin(th);
    set_y(y1, y2);

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;

        struct timespec start = woke;

        double v, w; get_u(&v,&w);
        double dt = PERIOD_ROBOT_NS / 1e9;
        get_x(&xc,&yc,&th);

        xc += dt * v * cos(th);
        yc += dt * v * sin(th);
        th += dt * w;
        if (th > PI) th -= TWO_PI;
        if (th < -PI) th += TWO_PI;
        set_x(xc, yc, th);

        y1 = xc + R*cos(th);
        y2 = yc + R*sin(th);
        set_y(y1, y2);

        double xr, yr; get_ref(&xr,&yr);
        traj_log(now_since_t0(&t0), y1, y2, xr, yr);

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_ROBOT, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_ROBOT_NS);
    }
    return NULL;
}
