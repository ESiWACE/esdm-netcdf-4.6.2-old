#!/bin/bash
set -eu
. osht.sh

#number of tests to run
PLAN 5

export NC_ESDM_FORCEESDM=1
export NC_ESDM_FORCEESDM_MKFS=1
export ESDM_LOGLEVEL_BUFFER=10

RUNS ./benchtool -f=esdm://longtest -w

RUNS ./benchtool -f=esdm://longtest -r

RUNS ./benchtool -f=esdm://longtest -w -r

RUNS mpiexec -np 2 ./src/benchtool -f="esdm://test.esdm" -n=1 -p=2
