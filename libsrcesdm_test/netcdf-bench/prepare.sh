#!/bin/bash -e

echo "Try to setup NetCDF benchmark for a system test"

if [[ "$1" == "clean" ]]; then
  rm -rf build benchtool
fi

if [[ ! -d netcdf-bench ]] ; then
  git clone https://github.com/joobog/netcdf-bench.git
fi

NETCDF_DIR="$PWD/../../install"

if [[ ! -d build ]] ; then
  mkdir build
fi

if [[ ! -e benchtool ]] ; then
  pushd build
  export PKG_CONFIG_PATH=$NETCDF_DIR/lib/pkgconfig/:$PKG_CONFIG_PATH
  cmake ../netcdf-bench -DNETCDF_INCLUDE_DIR=$NETCDF_DIR/include  -DNETCDF_LIBRARY=$NETCDF_DIR/lib/libnetcdf.so
  make -j 4
  cp ./src/benchtool  ../
  popd
fi

echo "[OK]"
