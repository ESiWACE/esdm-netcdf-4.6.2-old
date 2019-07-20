#!/bin/bash
cd ..
autoreconf
cd build
../configure --prefix=/home/kunkel/ur-git/esiwace/ESD-Middleware/install/ --with-esdm="/home/kunkel/ur-git/esiwace/ESD-Middleware/install" CFLAGS="-I/usr/include/hdf5/openmpi/ -g3" LDFLAGS="-L/usr/lib/x86_64-linux-gnu/hdf5/openmpi/" CC=mpicc  --disable-dap
