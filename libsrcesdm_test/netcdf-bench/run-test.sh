#!/bin/bash -e


B=./benchtool

$B -f=esdm://longtest

echo "Cleanup"
rm -rf _metadummy _esdm

echo "[OK]"
