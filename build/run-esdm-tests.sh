#!/bin/bash
pushd nc_test4/

for I in $(ls /libscresdm_test/ | grep "\.c$") ; do
  sed -i "s/NC_ESTRICTNC3/NC_NOERR/" ../../nc_test4/$I
  make ${I%%.c}
done

echo "Change benchmark"
sed "s/.*readfile_hdf5(file_name.*//" ../../nc_test4/tst_attsperf.c

for I in $(ls /libscresdm_test/ | grep "\.c$") ; do
  make ${I%%.c}
done

export NC_ESDM_FORCEESDM=1
export ESDM_LOGLEVEL_BUFFER=10
export NC_ESDM_FORCEESDM_MKFS=1

for I in $(ls /libscresdm_test/ | grep "\.c$") ; do
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
