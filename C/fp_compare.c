#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <endian.h>
#include <limits.h>
#include <math.h>

enum {
    DFLOAT_FRAC_BITS = 52,
    DFLOAT_EXP_BITS  = 11,
    DFLOAT_SIGN_BITS =  1,
    DFLOAT_FRAC_MASK = (1UL << DFLOAT_FRAC_BITS) - 1,
    DFLOAT_EXP_MASK  = (1UL << DFLOAT_EXP_BITS ) - 1,
    DFLOAT_SIGN_MASK = (1UL << DFLOAT_SIGN_BITS) - 1
};


static double DFLOAT_GEN(uint64_t sign, uint64_t exp, uint64_t frac);
static double DFLOAT_RANDOM(void);
static uint64_t DFLOAT_CONVERT(double d);
static void DFLOAT_EXTRACT(double d);
static int comparator_double(double d1, double d2);
static int comparator_uint64(uint64_t u1, uint64_t u2);



int main(int arg, char **argv)
{
    for (;;) {
        double   d1, d2;
        uint64_t u1, u2; 

        d1 = DFLOAT_RANDOM();
        d2 = DFLOAT_RANDOM();

        u1 = DFLOAT_CONVERT(d1);
        u2 = DFLOAT_CONVERT(d2);

        if (comparator_double(d1, d2) != comparator_uint64(u1, u2))
        {
            printf("double : %d\n", comparator_double(d1, d2));
            printf("convert: %d\n", comparator_uint64(u1, u2));

            DFLOAT_EXTRACT(d1);
            DFLOAT_EXTRACT(d2);

            printf("SIDE1: %f %016lx\n", d1, u1);
            printf("SIDE2: %f %016lx\n", d2, u2);
            exit(EXIT_SUCCESS);
        }
    }
           
    return 0;
}


static double DFLOAT_GEN(uint64_t sign, uint64_t exp, uint64_t frac)
{
    uint64_t d;

    d = ((DFLOAT_SIGN_MASK & sign) << (DFLOAT_FRAC_BITS + DFLOAT_EXP_BITS)) | 
        ((DFLOAT_EXP_MASK  & exp ) << (DFLOAT_FRAC_BITS)) |
        (DFLOAT_FRAC_MASK  & frac);

    return *(double*)&d;
}


static double DFLOAT_RANDOM(void)
{
    uint64_t sign, exp, frac;
    uint64_t exp_list[] = {0, 1, 1022, 1023, 1024,  2046, 2047};

    do {
        sign = rand() & DFLOAT_SIGN_MASK;
        exp  = exp_list[rand() % (sizeof(exp_list)/sizeof(exp_list[0]))];
        frac = ((uint64_t)(int64_t)((rand() % 32) - 16)) & DFLOAT_FRAC_MASK;
    } while (exp == 0 && frac != 0);

    return DFLOAT_GEN(sign, exp, frac);
}


static uint64_t DFLOAT_CONVERT(double d)
{
    uint64_t v    = *(uint64_t*)&d;

    uint64_t sign = DFLOAT_SIGN_MASK & (v >> (DFLOAT_FRAC_BITS + DFLOAT_EXP_BITS));
    uint64_t exp  = DFLOAT_EXP_MASK  & (v >> (DFLOAT_FRAC_BITS));
    uint64_t frac = DFLOAT_FRAC_MASK &  v;
    uint64_t n    = v & ~(1UL << 63);

    if (exp == 2047 && frac != 0) {
        return (uint64_t)-1;
    }

    return (1UL << 63) + (sign ? -n : n);
}


static void DFLOAT_EXTRACT(double d)
{
    uint64_t v    = *(uint64_t*)&d;

    uint64_t sign = DFLOAT_SIGN_MASK & (v >> (DFLOAT_FRAC_BITS + DFLOAT_EXP_BITS));
    uint64_t exp  = DFLOAT_EXP_MASK  & (v >> (DFLOAT_FRAC_BITS));
    uint64_t frac = DFLOAT_FRAC_MASK &  v;

    printf("D: %lu %lu %lu\n", sign, exp, frac);
}


static int comparator_double(double d1, double d2)
{
    if (isnan(d1)) {
        if (isnan(d2))
            return 0;      /* NAN = NAN */
        else
            return 1;      /* NAN > non-NAN */
    } else if (isnan(d2))
        return -1;         /* non-NAN < NAN */

    if (d1 == d2)
        return 0;
    else if (d1 > d2)
        return 1;
    else if (d1 < d2)
        return -1;
    else {
        printf("*** Not comparable ***\n");
        printf("%f, %f\n", d1, d2);
        DFLOAT_EXTRACT(d1);
        DFLOAT_EXTRACT(d2);
        exit(EXIT_FAILURE);
    }
}


static int comparator_uint64(uint64_t u1, uint64_t u2)
{
    if (u1 == u2)
        return 0;
    else if (u1 > u2)
        return 1;
    else
        return -1;
}


