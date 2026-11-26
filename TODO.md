# PION docs: TODO list

Started from a review of `docs/` against the PION `devel` source
(2026-11-17), and updated as the items were done (2026-11-20).

## Open

- [ ] Post-processing with TORUS: a section with example parameter files
      and run scripts (waiting for notes from JM).  `silo2fits`, which
      TORUS needs, is documented (#28).
- [ ] NEMO repository (not this repo): in `examples/minimal/README.md` and
      `examples/sundials-logger/README.md`, say which directory to run
      cmake from; add a note on building outside conda/venv environments
      to the top-level README (as in #27).

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
- [x] Troubleshooting page; shorter VisIt output in `example_sim.rst` (#22).
- [x] Power-of-2 check for the number of MPI processes (PION branch
      `nproc-power-of-2`, troubleshooting entry #23).
- [x] NEMO example: 1D HII region with 9 elements, parameter file,
      plotting script and figure (#24).
- [x] Stellar winds section of the developer guide (#25).
- [x] Jet inflow section: kept, with a warning that it is untested.
- [x] NEMO example in 2D: two merging HII regions, with parameter file,
      plotting script and figure (#24).
- [x] Running in parallel: `OMP_THREAD_LIMIT` and binding each MPI
      process to several cores (#26, with PION branch `omp-thread-limit`).
- [x] Compiling outside conda environments and Python venvs (#27).
- [x] Analysis tools page: `silocompare`, `silo2fits` (with TORUS),
      `silo2text`, `map-2d-to-3d` (#28, with PION branch
      `analysis-tools-fixes`, which fixes `silo2fits` and `map-2d-to-3d`
      and moves broken tools to `analysis/zz_deprecated/`).
- [x] Boundary conditions: the double Mach reflection boundaries (`DMR`,
      `DMR2`) are hard-coded, with no parameters.
- [x] Evolving stars page (plan 6.6): evolution tables, `STAR_<n>_evo_*`,
      SED switching with NEMO, timestep limits, `mesa2pion.py`; NEMO SED
      types 11 and 12; `EP_MP_init_equilibrium` and
      `WIND_[i]_equilibrium`; developer guide: evolving winds and the wind
      timestep limit (#32; `mesa2pion.py` v2 is PION !45).
- [x] NEMO's timestep criterion `EP_MP_timestep_limit 5` (NEMO 1.5 or
      later): recommended in modules.rst, parameter-file.rst,
      command_line.rst and troubleshooting.rst; both NEMO examples rerun
      with it (1D: 1720 instead of 2870 steps; 2D: 2150 instead of 21,600,
      same results), new figures.
- [x] Checked #26 and #28 against PION `devel` after the branches
      `omp-thread-limit` and `analysis-tools-fixes` were merged
      (2026-11-20): warnings, `silo2fits` arguments and text file, and the
      list of built tools match.
