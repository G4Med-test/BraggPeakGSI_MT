# BraggPeakGSI_MT

**Light-ion Bragg peak curves in water**: mean energy deposit per event for
proton, He3 and C12 beams, benchmarked against the GSI Helmholtz Centre for
Heavy Ion Research experimental setup (Schardt et al., GSI Scientific
Report 2007). Multithreaded (`_MT`) Geant4 application.

---

## 🔬 What this test does

Two ionization chambers (IC1 downstream of the exit window, IC2 upstream of
the water tank) measure the relative reading IC2/IC1 for pencil beams of
proton (125-200 MeV), He3 (110-225 MeV) and C12 (100-400 MeV/u), each with
its own beam-energy spread (`sigma(E)/E`) and scoring-depth window
(`LDEPTH`/`HDEPTH`), across 15 reference EM/hadronic physics lists
(QGSP_BIC_HP, Shielding, QBBC, each with EMY/EMZ/PEN/LIV variants).

## 📚 Reference

- Source: `geant-validation-tests/src/BraggPeakGSI_MT`.
- Schardt D. et al., GSI Scientific Report 2007.

## Import notes

- `main.cc`, `include/`, `src/` come from `BraggPeakGSI_MT.cc`/`include`/`src`.
- No concrete `.mac` files exist in the source tree — beams there are only
  defined as a `template.conf` + per-species `includes/{H,He,C}` fragments
  with `%BEAM%`/`%BEAM_ENERGY%`/`%SIGMA%`/`%LDEPTH%`/`%HDEPTH%`/`%PHYSLIST%`/
  `%NEVENTS%` placeholders, expanded at job-submission time by
  `geant-config-generator`. `macro/C12_400MeVu_QGSP_BIC_HP_EMY.mac` is a
  **synthesized, concrete example** built by substituting one row of
  `params.conf` (C12 @ 400 MeV/u, `QGSP_BIC_HP_EMY`) into `template.conf` +
  `includes/C`, so the repository ships at least one runnable macro. The
  other 14 params.conf rows (3 proton, 5 He3, 2 more C12 energies × up to 15
  physics lists) are not individually materialized — add more via the same
  substitution if needed. Its `/control/alias`/`/control/multiply`
  beam-energy computation was pre-resolved to literal numbers — see
  `validation/README.md`.
- `validation/parser.py` was **rewritten** for the `metadata()`/`parse()`
  module API that `ci-workflows/validation/export.py` actually calls (the
  first version, copied unmodified from
  `geant-config-generator/tests/geant4/BraggPeakGSI_MT/parser.py`, used the
  old `gts.BaseParser` class API and would have failed CI the same way
  `fanoCavity` initially did). See `validation/README.md` for details. The
  vendored `gts` package is no longer needed and was removed.
