#!/bin/sh

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"
deps_dir="${script_dir}/extra_libraries"
#modules_path="/path/to/modules"
build_dir="${script_dir}/build"

# (optionally) load any modules
module load cmake3
module load gcc
#module load gsl/gcc
module load openmpi/gcc/4.0.5

# Help Cmake find libraries if necessary
CMAKE_PREFIX_PATH="${modules_path}/gsl/gcc/2.5;${deps_dir};${CMAKE_PREFIX_PATH}"
SUNDIALS_DIR="${deps_dir}"
SILO_DIR="${deps_dir}"

mkdir -p ${build_dir}
pushd ${build_dir}

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPION_USE_SILO=ON \
    -DPION_NESTED_GRID=ON \
    -DSILO_DIR="${SILO_DIR}" \
    -DSUNDIALS_DIR="${SUNDIALS_DIR}" \
    "${script_dir}"
    #-DCMAKE_BUILD_TYPE=Debug \
    #-DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}" \
    #-DUSE_SILO=ON \

make -j 4

popd # build_dir
