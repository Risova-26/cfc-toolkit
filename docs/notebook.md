# Engineering Notebook — cfc-toolkit

## Day 1

**What I built:** Project scaffold.
force_model.h (interface), force_model.c (implementation),
CMakeLists.txt (build system), README.md, .gitignore, folder structure.

**Key physical insight:**
F = Kc*b*h + Ke*b has two terms because two mechanisms act simultaneously.
Shearing (chip formation) is proportional to chip thickness h.
Ploughing (edge rubbing) is constant — it sets a force floor at h=0.
At h=0: F = Ke*b = 15.0 N for default test parameters. This is the limiting case.

**Numbers:**
Kc=800 N/mm², Ke=15 N/mm, b=1.0 mm, h=0.057 mm (Test 3 feed rate from Priest 2024)
F_shearing  = 800 * 1.0 * 0.057 = 45.6 N
F_ploughing = 15 * 1.0          = 15.0 N
F_total                          = 60.6 N
Ploughing fraction: 15/60.6 = 24.8% of total force at this chip thickness.

**Unit derivation:**
Kc units: F = Kc*b*h → [N] = [Kc][mm][mm] → Kc in [N/mm²] ✓
Ke units: F = Ke*b   → [N] = [Ke][mm]     → Ke in [N/mm]  ✓

**Connection to paper:**
60.6 N at h=0.057 mm is physically consistent with Figure 5 in Priest 2024
(average forces of similar magnitude for Ti-6Al-4V at 2.4 mm radial depth).

**Puzzled by:** Nothing yet — model is clean. Tomorrow: why is chip thickness
h = feed_per_tooth × sin(engagement_angle) rather than just feed_per_tooth?

**Next:** Day 2 — compile force_model.c as object file, verify zero warnings,
complete G1-G4 gates.
