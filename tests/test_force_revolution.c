#include "force_model.h"

#include <stdio.h>

#define N_SAMPLES 8
#define GUARD_VALUE (-12345.0)

static int check_output(
    const double *values,
    const char *name)
{
    if (values[0] != GUARD_VALUE ||
        values[N_SAMPLES + 1] != GUARD_VALUE) {
        fprintf(stderr, "FAIL: %s guard value was overwritten\n", name);
        return 0;
    }

    for (int i = 1; i <= N_SAMPLES; ++i) {
        if (values[i] != 0.0) {
            fprintf(stderr,
                    "FAIL: %s[%d] expected 0.0, received %.6f\n",
                    name,
                    i - 1,
                    values[i]);
            return 0;
        }
    }

    return 1;
}

int main(void)
{
    Tool tool = {
        .D = 10.0,
        .Z = 4,
        .helix_deg = 30.0,
        .rake_deg = 10.0,
        .r_corner = 0.5
    };

    CuttingConditions cond = {
        .D_mm = 10.0,
        .Vc_m_min = 120.0,
        .Z = 4,
        .fz_mm = 0.05,
        .ae_mm = 2.0,
        .ap_mm = 5.0
    };

    ForceCoefficients coeffs = {
        .Kc = 1500.0,
        .Ke = 25.0
    };

    /*
     * The first and last elements are guard values. The function receives
     * only the N_SAMPLES elements between them.
     */
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

    if (!check_output(Fx, "Fx") ||
        !check_output(Fy, "Fy") ||
        !check_output(Fz, "Fz")) {
        return 1;
    }

    printf("PASS: force arrays initialised without crossing boundaries\n");
    return 0;
}