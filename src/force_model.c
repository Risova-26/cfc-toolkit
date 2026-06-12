/* force_model.c — Mechanistic cutting force model implementation
 *
 * Model: F = Kc * b * h  +  Ke * b
 *
 * Physical interpretation:
 *   Term 1 (Kc*b*h): shearing — energy to form the chip.
 *                    Proportional to chip cross-section area (b*h).
 *   Term 2 (Ke*b):   ploughing — rubbing of the non-sharp edge.
 *                    Independent of chip thickness, sets a force floor.
 *
 * Reference: Priest et al. 2024, Eq. simplified scalar form of RTA model.
 */

#include "force_model.h"
#include <stdio.h>

double predict_shearing(CuttingParams p, double h) {
    /* Unit check: [N/mm²] × [mm] × [mm] = [N/mm²] × [mm²] = [N] ✓ */
    return p.Kc * p.b * h;
}

double predict_ploughing(CuttingParams p) {
    /* Unit check: [N/mm] × [mm] = [N] ✓ */
    return p.Ke * p.b;
}

double predict_force(CuttingParams p, double h) {
    /* PHYSICS GATE — verify these mentally before every use:
     * Limiting case 1: h=0  → returns Ke*b (ploughing only)
     * Limiting case 2: Ke=0 → returns Kc*b*h (shearing only)
     * Linear check: doubling h should approximately double F
     *               when Ke*b is small relative to Kc*b*h */
    return predict_shearing(p, h) + predict_ploughing(p);
}

void print_force_table(CuttingParams p, double h_min, double h_max, int n_steps) {
    double step = (h_max - h_min) / (double)(n_steps - 1);

    printf("\n=== Force Table ===\n");
    printf("Parameters: Kc=%.1f N/mm2, Ke=%.1f N/mm, b=%.2f mm\n\n",
           p.Kc, p.Ke, p.b);
    printf("%-12s %-14s %-15s %-12s\n",
           "h [mm]", "Shearing [N]", "Ploughing [N]", "Total [N]");
    printf("%-12s %-14s %-15s %-12s\n",
           "--------", "------------", "-------------", "---------");

    for (int i = 0; i < n_steps; i++) {
        double h  = h_min + i * step;
        double Fs = predict_shearing(p, h);
        double Fp = predict_ploughing(p);
        double Ft = predict_force(p, h);
        printf("%-12.4f %-14.4f %-15.4f %-12.4f\n", h, Fs, Fp, Ft);
    }
    printf("\n");
}
