#include "force_model.h"

#include <math.h>
#include <stdio.h>

#define N_SAMPLES 360
#define GUARD_VALUE (-12345.0)
#define TOLERANCE 1.0e-9

static int nearly_equal(double actual, double expected)
{
    return fabs(actual - expected) <= TOLERANCE;
}

static int check_value(
    const char *name,
    double actual,
    double expected)
{
    if (!nearly_equal(actual, expected)) {
        fprintf(stderr,
                "FAIL: %s expected %.9f, received %.9f\n",
                name,
                expected,
                actual);
        return 0;
    }

    return 1;
}

static int check_guards(
    const double *values,
    const char *name)
{
    if (values[0] != GUARD_VALUE ||
        values[N_SAMPLES + 1] != GUARD_VALUE) {
        fprintf(stderr,
                "FAIL: %s wrote outside the output boundary\n",
                name);
        return 0;
    }

    return 1;
}

int main(void)
{
    Tool tool = {
        .D = 10.0,
        .Z = 1,
        .helix_deg = 0.0,
        .rake_deg = 10.0,
        .r_corner = 0.0
    };

    CuttingConditions cond = {
        .D_mm = 10.0,
        .Vc_m_min = 100.0,
        .Z = 1,
        .fz_mm = 0.1,
        .ae_mm = 5.0,
        .ap_mm = 2.0
    };

    ForceCoefficients coeffs = {
        .Ktc = 1000.0,
        .Krc = 500.0,
        .Kac = 200.0,
        .Kte = 10.0,
        .Kre = 5.0,
        .Kae = 2.0
    };

    double Fx[N_SAMPLES + 2];
    double Fy[N_SAMPLES + 2];
    double Fz[N_SAMPLES + 2];

    for (int i = 0; i < N_SAMPLES + 2; ++i) {
        Fx[i] = GUARD_VALUE;
        Fy[i] = GUARD_VALUE;
        Fz[i] = GUARD_VALUE;
    }

    compute_force_revolution(
        &tool,
        &cond,
        &coeffs,
        &Fx[1],
        &Fy[1],
        &Fz[1],
        N_SAMPLES
    );

    /*
     * Test 1: the function must not write before or after the arrays.
     */
    if (!check_guards(Fx, "Fx") ||
        !check_guards(Fy, "Fy") ||
        !check_guards(Fz, "Fz")) {
        return 1;
    }

    /*
     * Test 2: at 0 and 270 degrees, the single flute is outside engagement.
     */
    if (!check_value("Fx at 0 degrees", Fx[1], 0.0) ||
        !check_value("Fy at 0 degrees", Fy[1], 0.0) ||
        !check_value("Fz at 0 degrees", Fz[1], 0.0) ||
        !check_value("Fx at 270 degrees", Fx[271], 0.0) ||
        !check_value("Fy at 270 degrees", Fy[271], 0.0) ||
        !check_value("Fz at 270 degrees", Fz[271], 0.0)) {
        return 1;
    }

    /*
     * Test 3: at 90 degrees, h=fz=0.1 mm.
     *
     * Ft = (1000*0.1 + 10)*2 = 220 N
     * Fr = (500*0.1 + 5)*2   = 110 N
     * Fa = (200*0.1 + 2)*2   = 44 N
     *
     * At phi=90 degrees:
     * Fx=-Fr=-110 N, Fy=Ft=220 N, Fz=Fa=44 N.
     */
    if (!check_value("Fx at 90 degrees", Fx[91], -110.0) ||
        !check_value("Fy at 90 degrees", Fy[91], 220.0) ||
        !check_value("Fz at 90 degrees", Fz[91], 44.0)) {
        return 1;
    }

    /*
     * Test 4: a two-flute signal must repeat after half a revolution.
     */
    tool.Z = 2;
    cond.Z = 2;

    compute_force_revolution(
        &tool,
        &cond,
        &coeffs,
        &Fx[1],
        &Fy[1],
        &Fz[1],
        N_SAMPLES
    );

    const int comparison_samples[] = {
        15, 45, 75, 105, 135, 165
    };

    const int comparison_count =
        (int)(sizeof(comparison_samples) /
              sizeof(comparison_samples[0]));

    for (int i = 0; i < comparison_count; ++i) {
        const int first = comparison_samples[i];
        const int second = first + 180;

        if (!nearly_equal(Fx[first + 1], Fx[second + 1]) ||
            !nearly_equal(Fy[first + 1], Fy[second + 1]) ||
            !nearly_equal(Fz[first + 1], Fz[second + 1])) {
            fprintf(stderr,
                    "FAIL: two-flute periodicity failed at sample %d\n",
                    first);
            return 1;
        }
    }

    printf("PASS: revolution forces, transformation, periodicity and bounds\n");
    return 0;
}