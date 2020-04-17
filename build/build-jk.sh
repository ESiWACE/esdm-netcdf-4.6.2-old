#!/bin/bash
cd ..
autoreconf
cd build
PREFIX=/home/kunkel/ur-git/esiwace/ESD-Middleware/install/
../configure --prefix=$PREFIX --with-esdm=$PREFIX CFLAGS="-I/usr/include/hdf5/openmpi/ -g3" LDFLAGS="-L/usr/lib/x86_64-linux-gnu/hdf5/openmpi/" CC=mpicc  --disable-dap

exit 0
# To build NetCDFF later:

wget ftp://ftp.unidata.ucar.edu/pub/netcdf/netcdf-fortran-4.5.2.tar.gz
tar -xf netcdf-fortran-*
cd netcdf-fortran*
./configure --prefix=$PREFIX CFLAGS="-I/usr/include/hdf5/openmpi/ -g3 -I$PREFIX/include" LDFLAGS="-L/usr/lib/x86_64-linux-gnu/hdf5/openmpi/ -L$PREFIX/lib" CC=mpicc CPPFLAGS="-I$PREFIX/include" CC=mpicc FC=mpifort
