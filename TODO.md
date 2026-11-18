# PION docs: TODO list

Started from a review of `docs/` against the PION `devel` source
(2026-11-17), and updated as the items were done (2026-11-19).

## Open

- [ ] **Stellar winds developer page**: `docs/dev_winds.inc` is a one-line
      stub and is not included in the developer guide. Describe the wind
      classes (`stellar_wind_BC`, `stellar_wind_angle`,
      `stellar_wind_latdep`), evolving wind files, winds attached to stars,
      and winds across level boundaries.

## Done

- [x] Install and compilation instructions (#11).
- [x] PyPion page (#12).
- [x] Publications page (#13, #16).
- [x] Command-line options for `pion-*` and `icgen-*` (#15).
- [x] Microphysics reference: NEMO section, removed MP classes (MPv0, MPv1,
      MPv2, MPv4), MPv9 restored, more than one ionising source (#17).
- [x] Running in parallel: MPI plus OpenMP, decomposition, Slurm example,
      log files (#18).
- [x] Front page and Getting started: links to GitHub/readthedocs instead
      of git.dias.ie; obsolete files removed (`old/`, build scripts,
      `temp.text`, `usage/readme.rst`) (#18).
- [x] Parameter file reference: all `ics` types and their parameters,
      output and time control, tracers, all `EP_*` parameters, stars,
      winds, radiation sources; removed units and other unused parameters
      (with the matching PION changes) (#19).
- [x] Nested grids page, including radiation with nested grids and
      `NG_Mask` (#20).
- [x] Developer guide: current source layout, `Sub_domain` MPI code,
      workflow with `ci.skip` and clang-format-19, link to the Doxygen
      pages on GitLab Pages (#21).
- [x] Tests page: the `ctest` tests, how results are checked, CI (#21).
- [x] Troubleshooting page; shorter VisIt output in `example_sim.rst`.
- [x] Power-of-2 check for the number of MPI processes (PION branch
      `nproc-power-of-2`, troubleshooting entry #23).
- [x] NEMO example: 1D HII region with 9 elements, parameter file,
      plotting script and figure (`docs/example_nemo.rst`).
- [x] Jet inflow section: kept, with a warning that it is untested.
