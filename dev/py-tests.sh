#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 85

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10
export NC_ESDM_FORCEESDM_MKFS=1

RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_atts.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_cdf5.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_compoundvar.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_compression.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_create_mem.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_dap.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_dims.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_diskless.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_endian.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_fancyslicing.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_filepath.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_get_variables_by_attributes.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_grps2.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_grps.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_issue908.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked2.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked3.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked4.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked5.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked6.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_multifile2.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_multifile.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_netcdftime.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_open_mem.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_refcount.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_rename.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_scalarvar.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_scaled.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_shape.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_slicing.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_stringarr.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_types.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicode3.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicodeatt.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicode.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unlimdim.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unlimdimpy.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_Unsigned.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_utils.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_vars.py
RUNS mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_vlen.py
