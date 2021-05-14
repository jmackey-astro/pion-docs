#!/bin/sh

script="${BASH_SOURCE[0]:-${(%):-%x}}"
script_dir="$( cd "$( dirname "${script}" )" >/dev/null 2>&1 && pwd )"
build_dir="${script_dir}/build"
deps_dir="${script_dir}/extra_libraries"

mkdir -p ${build_dir}
pushd ${build_dir}

CMAKE_PREFIX_PATH="${deps_dir};${CMAKE_PREFIX_PATH}"

#    -DCMAKE_BUILD_TYPE=Release \
#    -DCMAKE_BUILD_TYPE=Debug \
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=mpicxx \
    -DPION_USE_SILO=ON \
    -DPION_USE_FITS=ON \
    -DPION_UNIFORM_GRID=ON \
    -DPION_NESTED_GRID=ON \
    -DPION_TOOLS=ON \
    -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}" \
    "${script_dir}"

make -j 4

#cmake \
#    -DCMAKE_BUILD_TYPE=Release \
#    -DCMAKE_CXX_COMPILER=g++ \
#    -DPION_USE_SILO=ON \
#    -DPION_UNIFORM_GRID=ON \
#    -DPION_NESTED_GRID=ON \
#    -DPION_PARALLEL=OFF \
#    "${script_dir}"
#
#make -j 4

popd # build_dir


