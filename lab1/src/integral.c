// src/integral.c
#include "integral.h"
#include <math.h>

static inline double _swap_if_needed(double *a, double *b) {
    if (*b >= *a) return 1.0;
    double tmp = *a; *a = *b; *b = tmp;
    return -1.0; // sinal do resultado
}

static inline int _validate(Func1D f, size_t n, IntegralStatus *st) {
    if (!f) { if (st) *st = INT_ERR_NULL_FUNC; return 0; }
    if (n == 0) { if (st) *st = INT_ERR_N_INVALID; return 0; }
    return 1;
}

// --- Riemann: pontos à esquerda ---
double riemann_left(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    if (!_validate(f, n, st)) return NAN;
    if (a == b) { if (st) *st = INT_OK; return 0.0; }
    double sign = _swap_if_needed(&a, &b);
    double h = (b - a) / (double)n;
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double x = a + i * h;
        sum += f(x, ctx);
    }
    if (st) *st = INT_OK;
    return sign * h * sum;
}

// --- Riemann: pontos à direita ---
double riemann_right(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    if (!_validate(f, n, st)) return NAN;
    if (a == b) { if (st) *st = INT_OK; return 0.0; }
    double sign = _swap_if_needed(&a, &b);
    double h = (b - a) / (double)n;
    double sum = 0.0;
    for (size_t i = 1; i <= n; ++i) {
        double x = a + i * h;
        sum += f(x, ctx);
    }
    if (st) *st = INT_OK;
    return sign * h * sum;
}

// --- Riemann: ponto médio (igual a Midpoint composto) ---
double riemann_midpoint(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    if (!_validate(f, n, st)) return NAN;
    if (a == b) { if (st) *st = INT_OK; return 0.0; }
    double sign = _swap_if_needed(&a, &b);
    double h = (b - a) / (double)n;
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double xm = a + (i + 0.5) * h;
        sum += f(xm, ctx);
    }
    if (st) *st = INT_OK;
    return sign * h * sum;
}

// --- Regra do Ponto Médio (composta) ---
double midpoint_rule(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    return riemann_midpoint(f, ctx, a, b, n, st);
}

// --- Regra do Trapézio (composta) ---
double trapezoidal_rule(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    if (!_validate(f, n, st)) return NAN;
    if (a == b) { if (st) *st = INT_OK; return 0.0; }
    double sign = _swap_if_needed(&a, &b);
    double h = (b - a) / (double)n;

    double sum = 0.5 * (f(a, ctx) + f(b, ctx));
    for (size_t i = 1; i < n; ++i) {
        double x = a + i * h;
        sum += f(x, ctx);
    }
    if (st) *st = INT_OK;
    return sign * h * sum;
}

// --- Regra de Simpson (composta) ---
double simpson_rule(Func1D f, void *ctx, double a, double b, size_t n, IntegralStatus *st) {
    if (!_validate(f, n, st)) return NAN;
    if (n % 2 != 0) { if (st) *st = INT_ERR_N_INVALID; return NAN; }
    if (a == b) { if (st) *st = INT_OK; return 0.0; }

    double sign = _swap_if_needed(&a, &b);
    double h = (b - a) / (double)n;

    // Simpson composto: S = h/3 [f(x0) + f(xn) + 4 Σ f(x_{odd}) + 2 Σ f(x_{even})]
    double sum = f(a, ctx) + f(b, ctx);

    double sum4 = 0.0; // ímpares
    double sum2 = 0.0; // pares

    for (size_t k = 1; k < n; ++k) {
        double xk = a + k * h;
        if (k % 2 == 0) sum2 += f(xk, ctx);
        else            sum4 += f(xk, ctx);
    }

    double S = (h / 3.0) * (sum + 4.0 * sum4 + 2.0 * sum2);
    if (st) *st = INT_OK;
    return sign * S;
}
