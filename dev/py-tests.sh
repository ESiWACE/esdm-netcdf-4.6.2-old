#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 85

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10
export NC_ESDM_FORCEESDM_MKFS=1

/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_atts.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_cdf5.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_compoundvar.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_compression.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_create_mem.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_dap.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_dims.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_diskless.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_endian.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_fancyslicing.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_filepath.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_get_variables_by_attributes.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_grps2.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_grps.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_issue908.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked2.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked3.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked4.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked5.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked6.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_masked.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_multifile2.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_multifile.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_netcdftime.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_open_mem.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_refcount.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_rename.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_scalarvar.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_scaled.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_shape.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_slicing.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_stringarr.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_types.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicode3.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicodeatt.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unicode.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unlimdim.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_unlimdimpy.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_Unsigned.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_utils.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_vars.py
/var/lib/jenkins/workspace/esdm/install/bin/mkfs.esdm --create --remove --ignore-errors -g -c _esdm.conf
RUNS python3 netcdf4-python/test/test_vlen.py
