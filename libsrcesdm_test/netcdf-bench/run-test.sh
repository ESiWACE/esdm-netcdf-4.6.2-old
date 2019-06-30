#!/bin/bash

./benchtool -f=esdm://longtest -w -r

echo "Cleanup"
rm -rf _metadummy _esdm

echo "[OK]"

