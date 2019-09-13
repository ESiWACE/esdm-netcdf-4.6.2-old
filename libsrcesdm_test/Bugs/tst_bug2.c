#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_bug2.nc"
#define INST_NAME "institution"
#define INSTITUTION "NCAR (National Center for Atmospheric \nResearch, Boulder, CO, USA)"
#define NDIMS_1 1
#define VAR_NAME "Britany"
#define WINSTON_CHURCHILL "Winston_S_Churchill"
#define D0_LEN 2

// We discussed this bug before, but I don't think the estabilished solution is working.

// tst_bug2: /home/lucy/esiwace/esdm/src/esdm-datatypes.c:382: esdmI_fragment_create: Assertion `sspace->size[i] > 0' failed.

int main(int argc, char **argv) {
  int ncid;
  int dimid, varid;
  // int data[D0_LEN] = {42, -42};
  int data[D0_LEN] = {42};

  /* Create a netcdf-4 file with 2D coordinate var. */
  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, WINSTON_CHURCHILL, NC_UNLIMITED, &dimid)) ERR;
  if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIMS_1, &dimid, &varid)) ERR;
  size_t start[1] = {0}, count[1] = {1};
  if (nc_put_vara_int(ncid, varid, start, count, data)) ERR;
  if (nc_put_var(ncid, varid, data)) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
