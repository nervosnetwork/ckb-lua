#include "../my_math.h"
#include <stdint.h>

#include "math_pow.c"
#include "math_log.c"

#ifndef DBL_EPSILON
#define DBL_EPSILON 2.22044604925031308085e-16
#endif

double acos(double x) { return 0; }

double asin(double x) { return 0; }

double atan2(double x, double y) { return 0; }

double cos(double x) { return 0; }

double cosh(double x) { return 0; }

double exp(double x) { return 0; }

#define EPS DBL_EPSILON
double floor(double x) {
    static const double toint = 1 / EPS;
    union {
        double f;
        uint64_t i;
    } u = {x};
    int e = u.i >> 52 & 0x7ff;
    double y;

    if (e >= 0x3ff + 52 || x == 0) return x;
    /* y = int(x) - x, where int(x) is an integer neighbor of x */
    if (u.i >> 63)
        y = x - toint + toint - x;
    else
        y = x + toint - toint - x;
    /* special case because of non-nearest rounding modes */
    if (e <= 0x3ff - 1) {
        FORCE_EVAL(y);
        return u.i >> 63 ? -1 : 0;
    }
    if (y > 0) return x + y - 1;
    return x + y;
}

double ceil(double x) {
    static const double toint = 1 / EPS;
    union {
        double f;
        uint64_t i;
    } u = {x};
    int e = u.i >> 52 & 0x7ff;
    double y;

    if (e >= 0x3ff + 52 || x == 0) return x;
    /* y = int(x) - x, where int(x) is an integer neighbor of x */
    if (u.i >> 63)
        y = x - toint + toint - x;
    else
        y = x + toint - toint - x;
    /* special case because of non-nearest rounding modes */
    if (e <= 0x3ff - 1) {
        FORCE_EVAL(y);
        return u.i >> 63 ? -0.0 : 1;
    }
    if (y < 0) return x + y + 1;
    return x + y;
}

double fabs(double x) {
    union {
        double f;
        uint64_t i;
    } u = {x};
    u.i &= -1ULL / 2;
    return u.f;
}

double ldexp(double x, int n) { return scalbn(x, n); }

double log2(double x) { return 0; }

double log10(double x) { return 0; }

double sin(double x) { return 0; }

double sinh(double x) { return 0; }

double sqrt(double x) { return 0; }

double tan(double x) { return 0; }

double tanh(double x) { return 0; }
