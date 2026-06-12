# cfc-toolkit

Offline Cutting Force Coefficient (CFC) identification and force prediction toolkit.
Implements the mechanistic modelling pipeline from:

> Priest, J. et al. (2024). Assessment of cutting force coefficient identification
> methods and force models for variable pitch and helix bull-nose tools.
> CIRP Journal of Manufacturing Science and Technology, 55, 210-223.

## What it does
1. Reads machining force data from CSV (simulating Kistler 9255C dynamometer output)
2. Segments signal into cutting and non-cutting regions
3. Identifies Kc [N/mm²] and Ke [N/mm] via least squares
4. Predicts cutting forces: F = Kc·b·h + Ke·b
5. Validates predictions (RMSE, MAE, residual plots)

## Architecture
| Layer    | Language | Purpose                                   |
|----------|----------|-------------------------------------------|
| Core     | C        | Force model, signal segmentation, CSV I/O |
| Interface| C++      | ForceModel class, CFCIdentifier class     |
| Binding  | pybind11 | Expose C++ to Python                      |
| Pipeline | Python   | Data loading, plotting, validation        |

## Build
cmake -S . -B build && cmake --build build

## Status
Day 1 — scaffold and force model interface.
