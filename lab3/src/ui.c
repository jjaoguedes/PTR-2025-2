
#include "ui.h"
#include "monitor.h"
#include "reference.h"
#include "time_utils.h"
#include <fcntl.h>
#include <sys/select.h>

static void print_status(const struct timespec *t0) {
    double t = now_since_t0(t0);
    double xc,yc,th,y1,y2,xr,yr,a1,a2; get_x(&xc,&yc,&th); get_y(&y1,&y2); get_ref(&xr,&yr); get_alpha(&a1,&a2);
    printf("[t=%6.2fs] pos=(%.3f, %.3f) th=%.3f rad | y=(%.3f, %.3f) | ref=(%.3f, %.3f) | alpha=(%.2f, %.2f)\n",
           t, xc, yc, th, y1, y2, xr, yr, a1, a2);
    fflush(stdout);
}

void *th_ui(void *arg) {
    struct timespec t0 = *(struct timespec*)arg;
    struct timespec next = t0; next = ts_add_ns(next, PERIOD_UI_NS);

    // stdin não bloqueante
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    printf("Comandos: 'a1=NUM', 'a2=NUM', 'alpha NUM NUM', 'q' para sair.\n");

    while (!get_stop()) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        struct timespec woke; clock_gettime(CLOCK_MONOTONIC, &woke);
        double jitter_ms = (double)ts_diff_ns(&woke, &next)/1e6;
        struct timespec start = woke;

        print_status(&t0);

        char buf[256]; int n = read(STDIN_FILENO, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            if (buf[0]=='q') { set_stop(true); break; }
            double a1,a2;
            if (sscanf(buf, "a1=%lf", &a1)==1) { double tmp; get_alpha(&tmp,&a2); set_alpha(a1, a2); }
            else if (sscanf(buf, "a2=%lf", &a2)==1) { double tmp; get_alpha(&a1,&tmp); set_alpha(a1, a2); }
            else if (sscanf(buf, "alpha %lf %lf", &a1, &a2)==2) { set_alpha(a1, a2); }
        }

        struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end);
        double comp_ms = (double)ts_diff_ns(&end, &start)/1e6;
        perf_log(TH_UI, now_since_t0(&t0), comp_ms, jitter_ms);

        next = ts_add_ns(next, PERIOD_UI_NS);
    }
    return NULL;
}

void *th_load(void *arg) {
    (void)arg;
    volatile double x = 0.0;
    while (!get_stop()) {
        for (int i=0;i<500000;i++) { x += sin(i); }
        struct timespec ts = {.tv_sec=0,.tv_nsec=1000000L}; // 1 ms
        nanosleep(&ts, NULL);
    }
    return NULL;
}
