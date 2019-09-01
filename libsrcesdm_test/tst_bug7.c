#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_bug7.nc"
#define NDIMS 3
#define NVARS 6
#define XC_NAME "xc"
#define YC_NAME "yc"
#define LEV_NAME "lev"
#define T_NAME "T"
#define XC_LEN 128
#define YC_LEN 64
#define LEV_LEN 18
#define DATA_NDIMS 3
#define COORD_NDIMS 2
#define SAMPLE_VALUE 0.5

int main(int argc, char **argv) {
  int ncid, dimids[NDIMS], varids[NVARS], data_dimids[DATA_NDIMS];
  int coord_dimids[COORD_NDIMS];
  float temp[YC_LEN][XC_LEN], xc[XC_LEN];
  size_t start[DATA_NDIMS] = {0, 0, 0}, count[DATA_NDIMS] = {1, YC_LEN, XC_LEN};
  int x, y;

  /* Initialize some fake data. */
  for (y = 0; y < YC_LEN; y++)
    for (x = 0; x < XC_LEN; x++)
      temp[y][x] = SAMPLE_VALUE;
  for (x = 0; x < XC_LEN; x++)
    xc[x] = SAMPLE_VALUE;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, XC_NAME, XC_LEN, &dimids[0])) ERR;
  if (nc_def_dim(ncid, YC_NAME, YC_LEN, &dimids[1])) ERR;
  if (nc_def_dim(ncid, LEV_NAME, LEV_LEN, &dimids[2])) ERR;
  data_dimids[0] = dimids[2];
  data_dimids[1] = dimids[1];
  data_dimids[2] = dimids[0];
  if (nc_def_var(ncid, T_NAME, NC_FLOAT, DATA_NDIMS, data_dimids, &varids[0])) ERR;
  if (nc_def_var(ncid, XC_NAME, NC_FLOAT, 1, &dimids[0], &varids[1])) ERR;

  if (nc_put_vara_float(ncid, varids[0], start, count, (const float *)temp)) ERR;
  if (nc_put_var_float(ncid, varids[1], xc)) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
