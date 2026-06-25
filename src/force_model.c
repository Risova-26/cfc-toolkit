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
#include <math.h>           /* sin() - requires -lm at link time */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif 

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

/*------------ Machining parameter functions -------------------*/

double calc_spindle_speed(double D_mm, double Vc_m_min) {
    if (D_mm <= 0.0 || Vc_m_min <= 0.0) return -1.0;
    return (Vc_m_min * 1000.0) / (M_PI * D_mm);
}

double calc_table_feed(double fz_mm, int Z, double N_rpm) {
    if (fz_mm <= 0.0 || Z < 1 || N_rpm <= 0.0) return -1.0;
    return fz_mm * (double)Z * N_rpm;
}

double calc_chip_thickness_simple(double fz_mm, double phi_rad) {
    /* h(phi) = fz x sin(phi). Zero or positive only -no negative chip.*/
    double h = fz_mm * sin(phi_rad);
    return (h > 0.0) ? h:0.0;
}

/* Engagement geometry fuctions */

/*  calc_phi_start: returns the angle [rad] where the tooth enters the workpiece.
    Formula: arccos((R-ae)/R)
    Physical meaning: the tooth first touch material when its circular path
    reaches the workpiece edge, which is (R-ae) from the tool centre. */
double calc_phi_start(double ae_mm, double R_mm){
    return acos((R_mm - ae_mm)/R_mm);
}

/*  calc_phi_exit: returns pi[rad] = 180 degrees.
    For up-milling the tooth exits at the centreline of the tool path.
    Parameters kept for future down-milling extension.*/
double calc_phi_exit(double ae_mm, double R_mm) {
    (void)ae_mm;    /* unused for up-milling - suppress - Wunused-parameter */
    (void)R_mm;
    return M_PI;
}

/*  is_engaged: returns 1 if the tooth is cutting at phi_rad, 0 if in air.
    Checks whether phi_rad falls inside [phi_start, phi_exit]. */
int is_engaged(double phi_rad, double phi_start, double phi_exit){
    return (phi_rad >= phi_start && phi_rad <= phi_exit) ? 1:0;
}

/*  calc_chip_thickness: angle-aware chip thickness [mm]
    Retruns fz* sin(phi) when engaged, 0.0 when outside the window.*/
double calc_chip_thickness(double fz_mm, double phi_rad,
                            double phi_start, double phi_exit){
    if (!is_engaged(phi_rad, phi_start, phi_exit)) return 0.0;
    return fz_mm * sin(phi_rad);
}