#include <config.h>
#include <nc_tests.h>
#include <nc4internal.h>
#include "err_macros.h"

#define FILE_NAME "tst_bug8.nc"
#define LAT_NAME "lat"
#define LON_NAME "lon"
#define LAT_LEN 1
#define LON_LEN 2
#define MAX_DIMS 5

// This is an example with NC_NAT and the conversion. It should work, but it's not implemented to work. We talked about it before.

int main(int argc, char **argv) {
  int ncid, dimids[MAX_DIMS];
  int lat_dimid, lon_dimid, lat_varid, lon_varid;
  float lat[LAT_LEN], lon[LON_LEN];
  float lat_in[LAT_LEN], lon_in[LON_LEN];
  int i, j;

  for (i = 0; i < LAT_LEN; i++)
    lat[i] = 40.0;
  for (i = 0; i < LON_LEN; i++)
    lon[i] = 20.0;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, LAT_NAME, LAT_LEN, &lat_dimid)) ERR;
  dimids[0] = lat_dimid;
  if (nc_def_var(ncid, LAT_NAME, NC_FLOAT, 1, dimids, &lat_varid)) ERR;
  if (nc_def_dim(ncid, LON_NAME, LON_LEN, &lon_dimid)) ERR;
  dimids[0] = lon_dimid;
  if (nc_def_var(ncid, LON_NAME, NC_FLOAT, 1, dimids, &lon_varid)) ERR;
  dimids[0] = lat_dimid;
  dimids[1] = lon_dimid;
  if (nc_put_var_float(ncid, lat_varid, lat)) ERR;
  if (nc_put_var_float(ncid, lon_varid, lon)) ERR;
  if (nc_get_var(ncid, lat_varid, lat_in)) ERR;
  for (i = 0; i < LAT_LEN; i++)
    if (lat[i] != lat_in[i]) ERR;
  if (nc_get_var_float(ncid, lon_varid, lon_in)) ERR;
  for (i = 0; i < LON_LEN; i++)
    if (lon[i] != lon_in[i]) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
