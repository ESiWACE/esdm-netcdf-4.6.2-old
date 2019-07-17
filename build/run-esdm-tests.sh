#!/bin/bash
pushd nc_test4/
for I in $(ls ../../nc_test4/ | grep "\.c$") ; do
  make ${I%%.c}
done

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10

for I in $(ls ../../nc_test4/ | grep "\.c$") ; do
  I=${I%%.c}
  if [[ -e $I ]] ; then
    echo "Running $I"
    ./$I
    if [[ $? != 0 ]] ; then
      break
    fi
    echo "OK"
    sleep 1
  fi
done

popd
