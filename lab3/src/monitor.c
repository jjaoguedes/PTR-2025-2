
#include "monitor.h"

Shared G;

void monitor_init(Shared *s) {
    memset(s, 0, sizeof(*s));
    s->alpha1 = 3.0; s->alpha2 = 3.0;
    pthread_mutex_init(&s->mtx, NULL);
}

#define LOCK   pthread_mutex_lock(&G.mtx)
#define UNLOCK pthread_mutex_unlock(&G.mtx)

void set_u(double v, double w) { LOCK; G.u1=v; G.u2=w; UNLOCK; }
void get_u(double *v, double *w) { LOCK; *v=G.u1; *w=G.u2; UNLOCK; }

void set_x(double xc, double yc, double th) { LOCK; G.xc=xc; G.yc=yc; G.theta=th; UNLOCK; }
void get_x(double *xc, double *yc, double *th) { LOCK; *xc=G.xc; *yc=G.yc; *th=G.theta; UNLOCK; }

void set_y(double y1, double y2) { LOCK; G.y1=y1; G.y2=y2; UNLOCK; }
void get_y(double *y1, double *y2) { LOCK; *y1=G.y1; *y2=G.y2; UNLOCK; }

void set_v(double vx, double vy) { LOCK; G.vx=vx; G.vy=vy; UNLOCK; }
void get_v(double *vx, double *vy) { LOCK; *vx=G.vx; *vy=G.vy; UNLOCK; }

void set_model_x(double ymx, double dymx) { LOCK; G.ymx=ymx; G.dymx=dymx; UNLOCK; }
void get_model_x(double *ymx, double *dymx) { LOCK; *ymx=G.ymx; *dymx=G.dymx; UNLOCK; }

void set_model_y(double ymy, double dymy) { LOCK; G.ymy=ymy; G.dymy=dymy; UNLOCK; }
void get_model_y(double *ymy, double *dymy) { LOCK; *ymy=G.ymy; *dymy=G.dymy; UNLOCK; }

void set_ref(double xr, double yr) { LOCK; G.xref=xr; G.yref=yr; UNLOCK; }
void get_ref(double *xr, double *yr) { LOCK; *xr=G.xref; *yr=G.yref; UNLOCK; }

void set_alpha(double a1, double a2) { LOCK; G.alpha1=a1; G.alpha2=a2; UNLOCK; }
void get_alpha(double *a1, double *a2) { LOCK; *a1=G.alpha1; *a2=G.alpha2; UNLOCK; }

void set_stop(bool v) { LOCK; G.stop=v; UNLOCK; }
bool get_stop(void) { bool r; LOCK; r=G.stop; UNLOCK; return r; }
