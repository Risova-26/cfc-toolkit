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

## Day 2

**What i built:** CuttingConditions struct in force_model.h
calc_spindle_speed(), calc_table_feed(), calc_chip_thickness_simple() in force_model.c machining calculator.c in amrc-prep.

**Key correction from plan.** Priest 2024 uses Vc=90 m/min not 60.
N=1790 RPM confirmed by hand calculation before running code.

**Numbers verified:**
- N  = 1790.5 RPM for D=16, Vc=90m/min
- Vf = 716.2 mm/min for fz=0.08mm/tooth, Z=5
- h(phi=90deg) = fz = 0.0800mm (maximum chip thickness)
- h(phi=0deg)  = 0.0000mm (no chip at tool entry - limiting case)
- Tooth-passing frequency = 149.2 Hz
- Kistler at 51200 Hz gives 172x Nyquist margin

**Physical insight:** h(phi) = fz x sin(phi). Chip thickness is NOT constant during a cut.
It varies sinusioidally - zero at entry, maximum at phi=90deg, zero at exit.
This is why cutting force oscillates at tooth-passing frequency.
The dynamometer must sample fast enough to capture these oscillations faithfully.

**Why link_libraries(m):** sin() lives in the C math library.
Unlike Python, C does not include it automatically. 
-lm tells the linker to include it. 
Without this the build fails even though the code compiles.

"A colleague says the tool is running at 2500 RPM with a 10mm diameter tool. What cutting speed Vc does this correspond to? Is this higher or lower than Priest's test conditions, and what does that imply for tool wear rate on Ti-6Al-4V?"
Ans: Vc = 2500 × π × 10 / 1000 = 78.54 m/min

This is lower than Priest's 90 m/min.

Tool wear rate is slower because:
- Lower cutting speed = less heat generated per unit time at the
  tool-chip interface
- Ti-6Al-4V has very low thermal conductivity (~7 W/mK vs ~50 for
  steel) so heat cannot escape into the workpiece — it stays in the
  tool and accelerates wear
- Less heat = slower chemical reaction between tool coating and
  titanium = slower wear rate

Trade-off: slower wear but also lower MRR (same fz, lower N means
lower Vf). The engineer is trading productivity for tool life.
This is a real decision made before every Ti-6Al-4V trial at AMRC.

**Next:** Day 3 - full engagement logic. calc_phi_start(),
calc_phi_exit(), is_engaged().
The angular window where the tool is actually cutting material.

## Day 3
Maximum h=fz always occurs at phi=90 degree. Midpoint of the engagement window is not point of maximum h. Both are inside the window for any real cut.

**What I built:**
- phi_start = 0.7954 rad = 45.6 deg for ae=2.4mm, D=16mm
- phi_exit = 3.1416 rad = 180.0 deg
- Engagement arc = 134.4 deg out of 360 deg
- h(90 deg) = 0.0800 mm = fz exactly
- h(0 deg) = h(270 deg) = 0.0000 mm (outside window)

**Assessment question:**
For Priest 2024 conditions (ae=2.4mm, D=16mm, fz=0.08mm/tooth):
- Enagement arc = 180 - 45.6 = 134.4 degrees
- Midpoint phi = (45.6 + 180) / 2 = 112.8 degrees
- h(112.8 deg) = 0.08 * sin(112.8 * pi/180) = 0.08 * 0.9219 = 0.0737 mm
- Tooth-passing frequency = Z * N / 60 = 5* 1790 / 60 = 149.2 Hz

**Physical insight:**
Maximum h = fz always occurs at phi=90 deg regardless of ae.
The midpoint of the engagement window (112.8 deg) is NOT the point of
maximum chip thickness. h is maximum where sin(phi)=1, not at the
geometric midpoint. The engagement arc (134.4 deg) is only 37% of a 
full revolution - the tool is cutting air for the other 63%.

**Parameters?**
Why does calc_phi_exit take ae and R as parameters if it ignores them?
Answer: consistency. Future down-milling implementation will need them.
Keeping the signature the same now means callers don't need to change later.

**Next action:**
Day 4 - Tool geometry structs (helix angle, rake angle, bull-nose radius)
and ForceCoefficients struct (Ktc, Krc, Kac, Kte, Kre, Kae).