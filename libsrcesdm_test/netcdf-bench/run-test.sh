#!/bin/bash

mkfs.esdm -g -l --create --remove
./benchtool -f=esdm://longtest -w -r

mkfs.esdm -g -l --create --remove
mpiexec -np 2 ./src/benchtool -f="esdm://test.esdm" -n=1 -p=2


echo "Cleanup"
rm -rf _metadummy _esdm

echo "[OK]"
