#include <config.h>
#include <nc_tests.h>
#include <nc4internal.h>
#include "err_macros.h"

#define FILE_NAME "tst_bug3.nc"
#define LAT_NAME "lat"
#define LAT_LEN 1
#define ELEV_NAME "Elevation"
#define LON_NAME "lon"
#define LON_LEN 2

// It cannot insert a variable ulonglong
// I have no idea what's going on here, too many problems!

int main(int argc, char **argv) {
  int ncid, lat_dimid, lon_dimid, elev_varid, dimids[2];
  unsigned long long elev, elev_in;

  /* Some phony elevaton data. */
  elev = 1010101022223333ULL;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, LAT_NAME, LAT_LEN, &lat_dimid)) ERR;
  if (nc_def_dim(ncid, LON_NAME, LON_LEN, &lon_dimid)) ERR;
  dimids[0] = lat_dimid;
  dimids[1] = lon_dimid;
  if (nc_def_var(ncid, ELEV_NAME, NC_INT64, 2, dimids, &elev_varid)) ERR;
  if (nc_put_var_ulonglong(ncid, elev_varid, (unsigned long long *)&elev)) ERR;
  if (nc_get_var_ulonglong(ncid, elev_varid, (unsigned long long *)elev_in)) ERR;
  if (elev != elev_in) ERR;
  if (nc_close(ncid)) ERR;

  SUMMARIZE_ERR;
  FINAL_RESULTS;
}
