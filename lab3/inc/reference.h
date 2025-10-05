
#ifndef REFERENCE_H
#define REFERENCE_H
#include "common.h"

void *th_reference(void *arg); // 120 ms

// equações de referência do enunciado
static inline double xref_func(double t) { return (5.0/PI) * cos(0.2*PI*t); }
static inline double yref_func(double t) {
    double s = (5.0/PI) * sin(0.2*PI*t);
    return (t < 10.0) ? s : -s;
}

#endif
