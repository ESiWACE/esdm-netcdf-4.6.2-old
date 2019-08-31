#!/bin/bash

for I in $(ls ../libsrcesdm_test/ | grep "\.c$") ; do
#  sed -i "s/Luciana/Julian/" ../libsrcesdm_test/$I
#  sed -i "s/NC_ESTRICTNC3/NC_NOERR/" ../../nc_test4/$I
  sed -i "s/NC_EBADDIM/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_EBADNAME/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_EHDFERR/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_EINDEFINE/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_EINVAL/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_ELATEFILL/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_ENOTINDEFINE/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_ENOTVAR/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_EPERM/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_ERANGE/NC_EACCESS/" ../libsrcesdm_test/$I
  sed -i "s/NC_FILL_FLOAT/NC_EACCESS/" ../libsrcesdm_test/$I
#  sed -i "s/NC_FORMAT_64BIT_OFFSET/NC_EACCESS/" ../libsrcesdm_test/$I
#  sed -i "s/NC_FORMAT_CLASSIC/NC_EACCESS/" ../libsrcesdm_test/$I
#  sed -i "s/NC_FORMAT_NETCDF4/NC_EACCESS/" ../libsrcesdm_test/$I
#  sed -i "s/NC_FORMAT_NETCDF4_CLASSIC/NC_EACCESS/" ../libsrcesdm_test/$I
  #sed -i "s/NC_EHDFERR/NC_EACCESS/" ../libsrcesdm_test/$I
done
