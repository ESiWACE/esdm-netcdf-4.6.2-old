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

// 1327	  ret = esdm_dataspace_subspace(space, ndims, size, offset, &subspace);
// (gdb)
// 1328	  if (ret != ESDM_SUCCESS) {
// (gdb)
// 1329	    return NC_EACCESS;
// (gdb) p ndims
// $1 = 1
// (gdb) p *space
// $2 = {type = 0x7ffff551c2e0, dims = 1, size = 0x5555558261a0, offset = 0x5555558261c0, stride = 0x0}
// (gdb) p *space->size
// $3 = 0
// (gdb)

int main(int argc, char **argv) {
  int ncid;
  int dimid, varid;
  // int data[D0_LEN] = {42, -42};
  int data[D0_LEN] = {42};

  /* Create a netcdf-4 file with 2D coordinate var. */
  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, WINSTON_CHURCHILL, NC_UNLIMITED, &dimid)) ERR;
  if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIMS_1, &dimid, &varid)) ERR;
  if (nc_put_var_int(ncid, varid, data)) ERR;
  // size_t start[1] = {0}, count[1] = {1};
  // if (nc_put_vara_int(ncid, varid, start, count, data)) ERR;
  if (nc_put_var(ncid, varid, data)) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
