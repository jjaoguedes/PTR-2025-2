
#include "common.h"
#include "monitor.h"
#include "time_utils.h"
#include "plant.h"
#include "linearization.h"
#include "control.h"
#include "ref_model.h"
#include "reference.h"
#include "ui.h"

static void *run_thread(void *(*fn)(void*), const char *name, struct timespec *t0, pthread_t *th) {
    (void)name;
    if (pthread_create(th, NULL, fn, t0) != 0) {
        perror(name); exit(1);
    }
    return NULL;
}

int main(int argc, char **argv) {
    bool with_load = false;
    if (argc > 1 && strcmp(argv[1], "--load")==0) with_load = true;

    monitor_init(&G);
    struct timespec t0; clock_gettime(CLOCK_MONOTONIC, &t0);

    // arquivos de log
    perf_open(with_load ? "out/perf_load.csv" : "out/perf_noload.csv");
    traj_open("out/traj.csv");

    // threads
    pthread_t thr_robot, thr_lin, thr_ctrl, thr_mx, thr_my, thr_ref, thr_ui, thr_load;

    run_thread(th_robot, TH_ROBOT, &t0, &thr_robot);
    run_thread(th_linearization, TH_LIN, &t0, &thr_lin);
    run_thread(th_control, TH_CTRL, &t0, &thr_ctrl);
    run_thread(th_model_x, TH_MX, &t0, &thr_mx);
    run_thread(th_model_y, TH_MY, &t0, &thr_my);
    run_thread(th_reference, TH_REF, &t0, &thr_ref);
    run_thread(th_ui, TH_UI, &t0, &thr_ui);

    if (with_load) run_thread(th_load, TH_LOAD, &t0, &thr_load);

    while (now_since_t0(&t0) < SIM_SECONDS && !get_stop()) {
        struct timespec ts = {.tv_sec=0, .tv_nsec=10000000L}; // 10 ms
        nanosleep(&ts, NULL);
    }
    set_stop(true);

    pthread_join(thr_robot, NULL);
    pthread_join(thr_lin, NULL);
    pthread_join(thr_ctrl, NULL);
    pthread_join(thr_mx, NULL);
    pthread_join(thr_my, NULL);
    pthread_join(thr_ref, NULL);
    pthread_join(thr_ui, NULL);
    if (with_load) pthread_join(thr_load, NULL);

    perf_close();
    traj_close();

    printf("Fim da simulação (%s).\n", with_load ? "com carga" : "sem carga");
    return 0;
}
