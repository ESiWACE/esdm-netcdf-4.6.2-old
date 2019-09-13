#include <config.h>
#include <nc_tests.h>
#include <nc4internal.h>
#include "err_macros.h"

#define FILE_NAME "tst_bug4.nc"
#define LAT_NAME "lat"
#define LON_NAME "lon"
#define LAT_LEN 1
#define ELEV_NAME "Elevation"
#define MAX_DIMS 5

// It cannot insert a variable ulonglong
// I have no idea what's going on here, too many problems!

int main(int argc, char **argv) {
  int ncid, lat_dimid, elev_varid, dimids[MAX_DIMS];
  int lat_varid, lon_dimid, lon_varid, pres_varid, hp_varid;
  unsigned long long elev, elev_in;
  char name_in[NC_MAX_NAME + 1]; // If remove it, goes to segfault!

  /* Some phony elevaton data. */
  elev = 1010101022223333ULL;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

  if (nc_def_dim(ncid, LAT_NAME, LAT_LEN, &lat_dimid)) ERR; // If remove it, goes back to the other error
  if (nc_def_var(ncid, LAT_NAME, NC_FLOAT, 1, dimids, &lat_varid)) ERR;
  if (nc_def_var(ncid, LON_NAME, NC_FLOAT, 1, dimids, &lon_varid)) ERR;
  if (nc_def_var(ncid, ELEV_NAME, NC_INT64, 2, dimids, &elev_varid)) ERR;
  if (nc_put_var_ulonglong(ncid, elev_varid, (unsigned long long *)&elev)) ERR;
  if (nc_get_var_ulonglong(ncid, elev_varid, (unsigned long long *)elev_in)) ERR;
  if (elev != elev_in) ERR;
  if (nc_close(ncid)) ERR;

  SUMMARIZE_ERR;
  FINAL_RESULTS;
}
