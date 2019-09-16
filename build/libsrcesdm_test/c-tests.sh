#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 30

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10
export NC_ESDM_FORCEESDM_MKFS=1

RUNS ./test_szip
RUNS ./tst_atts1
RUNS ./tst_atts2
RUNS ./tst_atts3
RUNS ./tst_atts
RUNS ./tst_attsperf
RUNS ./tst_atts_string_rewrite
RUNS ./tst_camrun
RUNS ./tst_converts
RUNS ./tst_coords2
RUNS ./tst_coords3
RUNS ./tst_coords
RUNS ./tst_create_files
RUNS ./tst_dims2
RUNS ./tst_dims
RUNS ./tst_files3
RUNS ./tst_files5
RUNS ./tst_files6
RUNS ./tst_files
RUNS ./tst_fill_attr_vanish
RUNS ./tst_fillbug
RUNS ./tst_fills
RUNS ./tst_filterparser
RUNS ./tst_large
RUNS ./tst_mem
RUNS ./tst_mode
RUNS ./tst_mpi_parallel
RUNS ./tst_parallel
RUNS ./tst_put_vars
RUNS ./tst_rehash
RUNS ./tst_rename2
RUNS ./tst_rename
RUNS ./tst_simplerw_coll_r
RUNS ./tst_sync
RUNS ./tst_unlim_vars
RUNS ./tst_v2
RUNS ./tst_varms
RUNS ./tst_vars2
RUNS ./tst_vars3
RUNS ./tst_vars4
RUNS ./tst_vars
RUNS ./t_type
