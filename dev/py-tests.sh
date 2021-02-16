#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 85

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10
#export NC_ESDM_FORCEESDM_MKFS=1
export PATH=/var/lib/jenkins/workspace/esdm/install/bin:$PATH

function mkfs(){
  mkfs.esdm --create --remove --ignore-errors -g
}

# Run all manual python tests
for t in python-esdm-tests/*; do
mkfs
RUNS python3 $t
done

mkfs
RUNS python3 netcdf4-python/test/test_atts.py
mkfs
RUNS python3 netcdf4-python/test/test_cdf5.py
mkfs
RUNS python3 netcdf4-python/test/test_compoundvar.py
mkfs
RUNS python3 netcdf4-python/test/test_compression.py
mkfs
RUNS python3 netcdf4-python/test/test_create_mem.py
mkfs
RUNS python3 netcdf4-python/test/test_dap.py
mkfs
RUNS python3 netcdf4-python/test/test_dims.py
mkfs
RUNS python3 netcdf4-python/test/test_diskless.py
mkfs
RUNS python3 netcdf4-python/test/test_endian.py
mkfs
RUNS python3 netcdf4-python/test/test_fancyslicing.py
mkfs
RUNS python3 netcdf4-python/test/test_filepath.py
mkfs
RUNS python3 netcdf4-python/test/test_get_variables_by_attributes.py
mkfs
RUNS python3 netcdf4-python/test/test_grps2.py
mkfs
RUNS python3 netcdf4-python/test/test_grps.py
mkfs
RUNS python3 netcdf4-python/test/test_issue908.py
mkfs
RUNS python3 netcdf4-python/test/test_masked2.py
mkfs
RUNS python3 netcdf4-python/test/test_masked3.py
mkfs
RUNS python3 netcdf4-python/test/test_masked4.py
mkfs
RUNS python3 netcdf4-python/test/test_masked5.py
mkfs
RUNS python3 netcdf4-python/test/test_masked6.py
mkfs
RUNS python3 netcdf4-python/test/test_masked.py
mkfs
RUNS python3 netcdf4-python/test/test_multifile2.py
mkfs
RUNS python3 netcdf4-python/test/test_multifile.py
mkfs
RUNS python3 netcdf4-python/test/test_netcdftime.py
mkfs
RUNS python3 netcdf4-python/test/test_open_mem.py
mkfs
RUNS python3 netcdf4-python/test/test_refcount.py
mkfs
RUNS python3 netcdf4-python/test/test_rename.py
mkfs
RUNS python3 netcdf4-python/test/test_scalarvar.py
mkfs
RUNS python3 netcdf4-python/test/test_scaled.py
mkfs
RUNS python3 netcdf4-python/test/test_shape.py
mkfs
RUNS python3 netcdf4-python/test/test_slicing.py
mkfs
RUNS python3 netcdf4-python/test/test_stringarr.py
mkfs
RUNS python3 netcdf4-python/test/test_types.py
mkfs
RUNS python3 netcdf4-python/test/test_unicode3.py
mkfs
RUNS python3 netcdf4-python/test/test_unicodeatt.py
mkfs
RUNS python3 netcdf4-python/test/test_unicode.py
mkfs
RUNS python3 netcdf4-python/test/test_unlimdim.py
mkfs
RUNS python3 netcdf4-python/test/test_unlimdimpy.py
mkfs
RUNS python3 netcdf4-python/test/test_Unsigned.py
mkfs
RUNS python3 netcdf4-python/test/test_utils.py
mkfs
RUNS python3 netcdf4-python/test/test_vars.py
mkfs
RUNS python3 netcdf4-python/test/test_vlen.py
