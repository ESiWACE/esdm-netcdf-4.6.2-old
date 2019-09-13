#!/bin/bash

echo "This script builds the NetCDF4 Python module with support for NetCDF4 ESDM"

echo "Check where nc-config is found: should be ESDM install dir"
which nc-config

echo "Building from scratch"

git clone https://github.com/Unidata/netcdf4-python.git

cd netcdf4-python

patch -p1 --dry-run < ../v1.patch

patch -p1 < ../v1.patch


# prerequisites to build netcdf4-python:
# * Make sure [numpy](http://www.numpy.org/) and [Cython](http://cython.org/) are
#  installed and you have [Python](https://www.python.org) 2.7 or newer.
# * Make sure [HDF5](http://www.h5py.org/) and netcdf-4 are installed, and the `nc-config` utility
#  is in your Unix PATH.
# * Run `python3 setup.py build`, then `python setup.py install` (with `sudo` if necessary).
# MPI4PY is necessary!
# pip3 install mpi4py

# for Ubuntu:
# apt-get install cython3 python3-numpy python3-mpi4py
# Make sure you purged the hdf5 serial version:
# sudo apt-get purge libhdf5-serial-dev libhdf5-dev

python3 setup.py install --user
# * To run all the tests, execute `cd test && python run_all.py`.

echo "Check that the right NetCDF library is used"
ldd build/lib.linux-x86_64-3.6/netCDF4/_netCDF4.cpython-36m-x86_64-linux-gnu.so | grep netcdf

echo "Setup ESDM as target for tempfiles"
cd test
sed -i 's#tempfile.NamedTemporaryFile(.*delete=False).name#"esdm://testfile'$RANDOM'"#'  *.py
sed -i 's#os.remove(.*#pass#'  *.py

exit 0
