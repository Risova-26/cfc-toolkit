/* force_model.h — Cutting force model declarations
 *
 * This header defines the interface to the mechanistic force model.
 * Other .c and .cpp files #include "force_model.h" to use these
 * without knowing the implementation details.
 *
 * This pattern mirrors how AMRC machine APIs work: you include
 * the vendor header (e.g. Heidenhain LSV/2), link their .so,
 * and call their C functions — same concept.
 *
 * Reference: Priest et al. 2024, CIRP J. Manuf. Sci. Technol. 55, 210-223
 */

#ifndef FORCE_MODEL_H
#define FORCE_MODEL_H

/* CuttingParams — all model parameters grouped in one struct.
 * In Week 3 this becomes a C++ class with methods.
 * A struct stores its members contiguously in memory —
 * this matters when passing data to machine interfaces. */
typedef struct {
    double Kc;  /* cutting/shearing coefficient  [N/mm²] */
    double Ke;  /* edge/ploughing coefficient    [N/mm]  */
    double b;   /* chip width                    [mm]    */
} CuttingParams;

/* predict_force — mechanistic model: F = Kc*b*h + Ke*b
 *
 * Parameters:
 *   p  : CuttingParams (Kc [N/mm²], Ke [N/mm], b [mm])
 *   h  : chip thickness [mm]
 * Returns:
 *   Predicted cutting force [N]
 *
 * Assumptions:
 *   - Linear chip-thickness dependence (valid for h < 0.15 mm)
 *   - Constant b across this evaluation point
 *   - No thermal, runout, or dynamic deflection effects
 *
 * Limiting cases (verify these in tests):
 *   h = 0  → F = Ke * b  (ploughing only)
 *   Ke = 0 → F = Kc * b * h  (shearing only)
 */
double predict_force    (CuttingParams p, double h);
double predict_shearing (CuttingParams p, double h);  /* Kc*b*h [N] */
double predict_ploughing(CuttingParams p);            /* Ke*b   [N] */

/* print_force_table — print formatted force vs chip thickness table to stdout
 *
 * Parameters:
 *   p       : CuttingParams
 *   h_min   : minimum chip thickness [mm]
 *   h_max   : maximum chip thickness [mm]
 *   n_steps : number of rows in table
 */
void print_force_table(CuttingParams p, double h_min, double h_max, int n_steps);

#endif /* FORCE_MODEL_H */
