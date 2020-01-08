#!/bin/bash	
pushd libsrcesdm_test/ || exit 1

rm tst*
for I in $(ls ../../libsrcesdm_test/ | grep "\.c$") ; do	
  make ${I%%.c}	
done	

export NC_ESDM_FORCEESDM=1	
export NC_ESDM_FORCEESDM_MKFS=1	
export ESDM_LOGLEVEL_BUFFER=10	

popd
