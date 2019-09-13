/*
 * Test more renames of vars and dims.
 *
 * Ed Hartnett
 */

#include "err_macros.h"
#include "nc_tests.h"

#define TEST_NAME "tst_rename"
#define LAT "lat"
#define LON "lon"
#define LEV "lev"
#define DIM_X "x"
#define DIM_Y "y"
#define DIM_Z "z"

#define DIM1_LEN 4
#define NDIM1 1
#define NDIM3 3
#define NUM_ENDDEF_SETTINGS 2

int main(int argc, char **argv) {
  char filename[NC_MAX_NAME + 1];
  int ncid, dimid[NDIM3], varid[NDIM3];
  int dimid_in;
  int lat_data[DIM1_LEN] = {0, 1, 2, 3};
  int lon_data[DIM1_LEN] = {0, 10, 20, 30};
  int lev_data[DIM1_LEN] = {0, 100, 200, 300};

  if (nc_set_default_format(NC_EACCESS, NULL))
    ERR;

  sprintf(filename, "%s_data_%d.nc", TEST_NAME, NC_EACCESS);

  /* Create file with three dims. */
  if (nc_create(filename, 0, &ncid))
    ERR;
  if (nc_def_dim(ncid, LAT, DIM1_LEN, &dimid[0]))
    ERR;
  if (nc_def_dim(ncid, LON, DIM1_LEN, &dimid[1]))
    ERR;
  if (nc_def_dim(ncid, LEV, DIM1_LEN, &dimid[2]))
    ERR;

  /* Define coordinate data vars. */
  if (nc_def_var(ncid, LAT, NC_INT, NDIM1, &dimid[0], &varid[0]))
    ERR;
  if (nc_def_var(ncid, LON, NC_INT, NDIM1, &dimid[1], &varid[1]))
    ERR;
  if (nc_def_var(ncid, LEV, NC_INT, NDIM1, &dimid[2], &varid[2]))
    ERR;

  if (nc_enddef(ncid))
    ERR;

  if (nc_put_var(ncid, 0, lat_data))
    ERR;
  if (nc_put_var(ncid, 1, lon_data))
    ERR;
  if (nc_put_var(ncid, 2, lev_data))
    ERR;

  if (nc_close(ncid))
    ERR;

   FINAL_RESULTS;
}
