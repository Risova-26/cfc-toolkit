#include "force_model.h"

#include <stdio.h>
#include <stdlib.h>

#define N_SAMPLES 720

int main(int argc, char *argv[])
{
    const char *output_path = "results/force_revolution.csv";

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [output.csv]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        output_path = argv[1];
    }

    const Tool tool = {
        .D = 10.0,
        .Z = 4,
        .helix_deg = 30.0,
        .rake_deg = 10.0,
        .r_corner = 0.5
    };

    const CuttingConditions cond = {
        .D_mm = 10.0,
        .Vc_m_min = 120.0,
        .Z = 4,
        .fz_mm = 0.05,
        .ae_mm = 2.0,
        .ap_mm = 5.0
    };

    const ForceCoefficients coeffs = {
        .Ktc = 1000.0,
        .Krc = 500.0,
        .Kac = 200.0,
        .Kte = 10.0,
        .Kre = 5.0,
        .Kae = 2.0
    };

    double Fx[N_SAMPLES];
    double Fy[N_SAMPLES];
    double Fz[N_SAMPLES];

    compute_force_revolution(
        &tool,
        &cond,
        &coeffs,
        Fx,
        Fy,
        Fz,
        N_SAMPLES
    );

    FILE *file = fopen(output_path, "w");

    if (file == NULL) {
        perror("Unable to open output CSV");
        return EXIT_FAILURE;
    }

    if (fprintf(file, "sample,angle_deg,Fx_N,Fy_N,Fz_N\n") < 0) {
        fprintf(stderr, "Failed to write CSV header\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < N_SAMPLES; ++i) {
        const double angle_deg =
            360.0 * (double)i / (double)N_SAMPLES;

        if (fprintf(
                file,
                "%d,%.6f,%.6f,%.6f,%.6f\n",
                i,
                angle_deg,
                Fx[i],
                Fy[i],
                Fz[i]) < 0) {
            fprintf(stderr, "Failed while writing CSV row %d\n", i);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    if (fclose(file) != 0) {
        perror("Failed to close output CSV");
        return EXIT_FAILURE;
    }

    printf(
        "Wrote %d force samples to %s\n",
        N_SAMPLES,
        output_path
    );

    return EXIT_SUCCESS;
}