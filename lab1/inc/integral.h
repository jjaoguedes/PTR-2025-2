// inc/integral.h
#ifndef INTEGRAL_H
#define INTEGRAL_H

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

// Função-alvo: f(x, ctx) -> double
typedef double (*Func1D)(double x, void *ctx);

typedef enum {
    INT_OK = 0,
    INT_ERR_N_INVALID,   // n = 0, ou (no Simpson) n ímpar
    INT_ERR_NULL_FUNC    // ponteiro de função nulo
} IntegralStatus;

// Util: retorna 0 em [a,a], suporta a>b (resultado com sinal correto)

// --- Somas de Riemann ---
double riemann_left     (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st);
double riemann_right    (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st);
double riemann_midpoint (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st); // ponto médio

// --- Newton-Cotes compostas ---
double midpoint_rule    (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st); // igual à riemann_midpoint
double trapezoidal_rule (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st);
double simpson_rule     (Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st); // n deve ser PAR

#ifdef __cplusplus
}
#endif
#endif // INTEGRAL_H
