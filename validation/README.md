# Portal JSON parser

Rewritten from `geant-config-generator/tests/geant4/BraggPeakGSI_MT/parser.py`
(old `gts.BaseParser` API, already single-job — no job-group merge to drop
here) for the module-level `metadata()`/`parse()` API that
`ci-workflows/validation/export.py` actually calls. `read_output()` (column
parsing of the captured `output_0.out`) is otherwise unchanged.

`macro/C12_400MeVu_QGSP_BIC_HP_EMY.mac` originally (see the main
`README.md`) came out non-self-contained: it used `/control/alias`
(`energyPerUma`, `carbonMass`) and `/control/multiply` to compute the beam's
mean/sigma energy from `%BEAM_ENERGY%`/`%SIGMA%` at macro-run time, which
`read_macro()` rejects (`/control/alias`, unresolved `{totalE}`/`{sigmaE}`).
The macro now has those two values **pre-computed in Python** at generation
time (`totalE = 400 MeV/u × 12.0 u = 4800.0 MeV`,
`sigmaE = totalE × 0.0003 = 1.44 MeV`) and written as literal
`/beam/energy/mean`/`/beam/energy/sigma` commands instead — numerically
identical, just resolved ahead of time rather than by the macro interpreter.

`metadata()` derives `BEAM` from `/beam/ion 6 12 0` → `"C12"` (the only beam
this macro set generates so far); extend the mapping if proton/He3 macros
are added later the same way.
