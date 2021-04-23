#!/bin/sh
#
# Use e.g. Homebrew to install: cmake, boost, sundials, open-mpi, cfitsio
# cd to extra_libraries/ and run `bash install_silo.sh` to compile
# the SILO libraries.
# Then run this build script.

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"
deps_dir="${script_dir}/extra_libraries"
build_dir="${script_dir}/build"

mkdir -p ${build_dir}
pushd ${build_dir}

SILO_DIR="${deps_dir}"

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DPION_USE_SILO=ON \
    -DSILO_DIR="${SILO_DIR}" \
    -DPION_UNIFORM_GRID=ON \
    -DPION_NESTED_GRID=ON \
    "${script_dir}"

make -j 4

popd # build_dir
