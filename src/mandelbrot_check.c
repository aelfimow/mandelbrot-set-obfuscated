#include "mandelbrot_check.h"

struct number
{
    double re;
    double im;
};

static struct number compute(struct number zn, struct number c);
static double distance(struct number value);

int mandelbrot_check(double re, double im)
{
    struct number c = { .re = re, .im = im };
    struct number zn = { .re = 0.0, .im = 0.0 };
    struct number zn1 = { .re = 0.0, .im = 0.0 };
    double d = 0.0;
    void *label = 0;
    int count = 0;

loop_start:
    zn1 = compute(zn, c);

    d = distance(zn1);

    label = (d > 4.0) ? &&limit_reached : &&limit_not_reached;
    goto *label;

limit_not_reached:
    zn = zn1;

    ++count;

    label = (count < 100) ? &&loop_start : &&loop_end;
    goto *label;

loop_end:
    return 1;

limit_reached:
    return 0;
}

static struct number compute(struct number zn, struct number c)
{
    struct number result = { .re = 0.0, .im = 0.0 };

    result.re = (zn.re * zn.re) - (zn.im * zn.im) + c.re;

    result.im = (2.0 * zn.re * zn.im) + c.im;

    return result;
}

static double distance(struct number value)
{
    const double result = (value.re * value.re) + (value.im * value.im);

    return result;
}
