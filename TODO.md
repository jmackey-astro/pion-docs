# PION docs: TODO list

From a light review of `docs/` against the PION `devel` source (2026-11-17).
Only the structure, file paths, parameter names and module lists were
checked against the code; the text and the physics were not reviewed.

## Needs major updates

- [ ] **Microphysics reference** (`docs/modules.rst`)
  - [ ] Add a NEMO section (`chem_code NEMO`): multi-ion non-equilibrium
        microphysics, its `EP_nemo_*` parameters, elements and ions.
  - [ ] MPv9 is listed as a production module and in the `chem_code`
        example, but it is not compiled (`EXCLUDE_HD_MODULE`) and `MPv9.h`
        no longer exists in the source.
  - [ ] MPv0, MPv1, MPv2 and MPv4 are described as "legacy" but have been
        removed from the code.
  - [ ] MPv3, MPv5, MPv6 and MPv7 support more than one ionising source:
        document this.
- [ ] **Developer guide** (`docs/developers_guide.rst`, `docs/dev_mpi.inc`,
      `docs/dev_coding.inc`)
  - [ ] Remove references to files that no longer exist: `main_MPI.cpp`,
        `main_NG.cpp`, `main_NG_MPI.cpp`, `source/comms/`,
        `decomposition/MCMD_control.cpp`. MPI code is now in
        `source/sub_domain/`.
  - [ ] Check the domain-decomposition description (recursive splitting
        into 2^i domains) against `sub_domain`.
  - [ ] Update the git workflow (git.dias.ie, old CI pipeline) and add
        clang-format-19, `tools/format.sh` and `ctest`. Most of this can
        come from PION's `AGENTS.md`.
- [ ] **Parameter file reference** (`docs/parameter-file.rst`)
  - [ ] `ics` table is missing `ShockTube`, the `StarBench_*` tests,
        `Supernova`, `Clump_Spherical`/`Clump_Axisymmetric`,
        `LaserAblation3D`, `ReadBBTurbulence`, `BlastWave_File`, ...
        (see `source/ics/icgen_base.cpp`).
  - [ ] Remove stale notes in the `RT_*` table ("switch to 5 in next
        release UPDATE CODES!", "set to 1 unless using MPv10"); describe
        multiple energy bins for NEMO.
  - [ ] Units section documents parameters that "do not do anything".
- [ ] **Front page** (`docs/index.rst`): says the current version is
      2.0.0, with links to git.dias.ie and `pion.ie/docs/dev/`. Dev docs
      are now on readthedocs; pion.ie calls them "PION v.3".
- [ ] **Getting started** (`docs/getting_started.rst`):
  - [ ] "Public release" and "Getting help" still point to git.dias.ie
        and pion.ie.
  - [ ] The requirements section says Silo 4.11; it should say 4.11 or
        4.12.

## Missing topics

- [ ] **Command-line options** for `pion-*` and `icgen-*`: `omp-nthreads=`,
      `redirect=`, `maxwalltime=`, `finishtime=`, `opfreq=`,
      `opfreq_time=`, `optype=`, `outfile=`, `cfl=`, `cooling=`, and the
      other overrides.
- [ ] **Output and time control parameters**: `OutputFile`,
      `OutputFrequency`, `typeofop`, `StartTime`, `FinishTime`,
      `opfreq_time`, checkpointing.
- [ ] **Extra physics (`EP_*`) parameters**: about 25 exist (including
      `EP_nemo_*`, `EP_tc_strength`, `EP_wind_acceleration`,
      `EP_MP_timestep_limit`); only `EP_cooling`, `EP_chemistry` and
      `EP_raytracing` appear, and only in examples.
- [ ] **Tracers and species setup** (`ntracer`, `Tracer...`), and how
      NEMO's elements and ions are given in the parameter file (element
      mass fractions must sum to exactly 1).
- [ ] **Nested grids**: `docs/nested_grid.rst` is a stub and not in any
      table of contents. Cover choosing levels, `NG_Mask`, and the
      requirement that refined levels line up with MPI sub-domains.
- [ ] **Running in parallel**: MPI plus OpenMP, choosing ranks and
      threads, per-rank log files from `redirect=`.
- [ ] **Radiation with nested grids**: sources outside a refined level;
      restrictions on sources outside level 0 and sources at infinity.
- [ ] **Tests**: list the current tests and how to run them with `ctest`
      (the README's `make test-*` list is outdated).
- [ ] **A NEMO example**, e.g. based on `test/hii40_nemo.sh` or
      `test/rhd-2d-nemo-2src.sh`.
- [ ] **Stellar winds developer page**: `docs/dev_winds.inc` is a
      one-line stub.

Priority: new pages for command-line options, NEMO setup, and running in
parallel.

## Obsolete, could be removed

- [ ] MPv0, MPv1, MPv2, MPv4 sections in `docs/modules.rst`; remove MPv9
      or mark it as not compiled. MPv8 still exists, but only under
      `LEGACY_CODE`.
- [ ] `docs/build_scripts/build-*.sh` (Debian 10, Kay/ICHEC, old OS X):
      no longer linked from the docs.
- [ ] `docs/temp.text`: old Docker paragraph, not in any page.
- [ ] `docs/usage/readme.rst`: 2021 copy of the PION README with
      git.dias.ie links and the old test list. Replace it, or drop it and
      link to the repo README.
- [ ] `old/`: partial copy of the pion.ie WordPress site plus a sync
      script. Remove if pion.ie is no longer updated from here.
- [ ] Jet inflow section in `docs/parameter-file.rst` (last used in
      2010): keep with a warning, or cut.
- [ ] VisIt 3.1.1 screen output with old local paths in
      `docs/example_sim.rst`: could be shortened.

## Checked and fine

- Example parameter files match `test/problems/` in PION.
- Documented boundary conditions match the code.
- Install, Python and publications pages were updated in #11–#13.
- Old Sphinx warnings to fix: `html_static_path` points to a missing
  `nstatic` directory; `nested_grid.rst` is not in any table of contents.
