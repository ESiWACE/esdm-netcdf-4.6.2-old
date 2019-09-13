#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_bug6.nc"
#define VAR_NAME "Britany"
#define NDIMS 2
#define TEXT_LEN 15
#define D0_NAME "time"
#define D1_NAME "tl"

// NetCDF inserts two variables with the same name, different type, and that will have the same id.
// ESDM does not allow two variables with the same name. This might be a big restriction.

int main(int argc, char **argv) {
  int ncid, nvars_in, varids_in[1];
  int time_dimids[NDIMS], time_id;

  if (nc_create(FILE_NAME, NC_NETCDF4 | NC_CLASSIC_MODEL, &ncid)) ERR;
  if (nc_def_dim(ncid, D0_NAME, NC_UNLIMITED, &time_dimids[0])) ERR;
  if (nc_def_dim(ncid, D1_NAME, TEXT_LEN, &time_dimids[1])) ERR;
  if (nc_def_var(ncid, VAR_NAME, NC_USHORT, NDIMS, time_dimids,
      &time_id)
      != NC_NOERR) ERR;
  if (nc_def_var(ncid, VAR_NAME, NC_CHAR, NDIMS, time_dimids, &time_id)) ERR;
  if (nc_close(ncid)) ERR;

  SUMMARIZE_ERR;
  FINAL_RESULTS;
}
