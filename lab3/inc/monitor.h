
#ifndef MONITOR_H
#define MONITOR_H
#include "common.h"

typedef struct {
    // estados do robô
    double xc, yc, theta;     // x_c, y_c, θ
    double y1, y2;            // saída (frente do robô)
    // entradas
    double u1, u2;            // u = [v, ω]
    double vx, vy;            // v = [v_x, v_y] (após linearização ẏ = v)
    // modelo de referência
    double ymx, ymy;          // saídas
    double dymx, dymy;        // derivadas
    // referência
    double xref, yref;
    // ganhos
    double alpha1, alpha2;
    // controle de execução
    bool stop;
    // mutex
    pthread_mutex_t mtx;
} Shared;

extern Shared G;

void monitor_init(Shared *s);

// getters/setters (operações atômicas sob o mutex)
void set_u(double v, double w);
void get_u(double *v, double *w);

void set_x(double xc, double yc, double th);
void get_x(double *xc, double *yc, double *th);

void set_y(double y1, double y2);
void get_y(double *y1, double *y2);

void set_v(double vx, double vy);
void get_v(double *vx, double *vy);

void set_model_x(double ymx, double dymx);
void get_model_x(double *ymx, double *dymx);

void set_model_y(double ymy, double dymy);
void get_model_y(double *ymy, double *dymy);

void set_ref(double xr, double yr);
void get_ref(double *xr, double *yr);

void set_alpha(double a1, double a2);
void get_alpha(double *a1, double *a2);

void set_stop(bool v);
bool get_stop(void);

#endif
