#!/bin/bash


B=./benchtool

$B -f=esdm://longtest -w
$B -f=esdm://longtest -r

echo "Cleanup"
rm -rf _metadummy _esdm

echo "[OK]"
