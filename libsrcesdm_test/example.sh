#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 17

export NC_ESDM_FORCEESDM=1
#export ESDM_LOGLEVEL_BUFFER=10

RUNS ../../build/libsrcesdm_test/tst_atts2
RUNS ../../build/libsrcesdm_test/tst_atts_NOTWORKING
RUNS ../../build/libsrcesdm_test/tst_atts_string_rewrite
RUNS ../../build/libsrcesdm_test/tst_camrun
RUNS ../../build/libsrcesdm_test/tst_converts
RUNS ../../build/libsrcesdm_test/tst_files3
RUNS ../../build/libsrcesdm_test/tst_files5
RUNS ../../build/libsrcesdm_test/tst_filterparser
RUNS ../../build/libsrcesdm_test/tst_mem
RUNS ../../build/libsrcesdm_test/tst_mode
RUNS ../../build/libsrcesdm_test/tst_mpi_parallel.bin
RUNS ../../build/libsrcesdm_test/tst_mpi_parallel
RUNS ../../build/libsrcesdm_test/tst_parallel
RUNS ../../build/libsrcesdm_test/tst_put_vars
RUNS ../../build/libsrcesdm_test/tst_simplerw_coll_r
RUNS ../../build/libsrcesdm_test/tst_v2
RUNS ../../build/libsrcesdm_test/t_type



# you can mark known failures as TODO tests
# where you aspire to fix these and document
# the intended use case
#TODO IS $((1 + 1)) = 3
#TODO RUNS false
