#!/bin/bash


./benchtool -f=esdm://longtest -w -r -d=1:10:10:10

echo "Cleanup"
rm -rf _metadummy _esdm

echo "[OK]"
