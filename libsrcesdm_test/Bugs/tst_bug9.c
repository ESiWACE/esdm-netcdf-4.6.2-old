#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include "netcdf_f.h"

#define FILE_NAME "tst_bug9.nc"
#define NUM_DIMS 1
#define NUM_VARS 3
#define NDIMS 3
#define VAR_DIMS 3
#define DIM_A "dim1"
#define DIM_A_LEN 4
#define DIM_B "dim2"
#define DIM_B_LEN 3
#define DIM_C "dim3"
#define DIM_C_LEN NC_UNLIMITED
#define CXX_VAR_NAME "P"
#define LONG_NAME "long_name"
#define PRES_MAX_WIND "pressure at maximum wind"
#define UNITS "units"

// if (datatype == NC_STRING) {
//   new = smd_attr_new(name, etype, *(void **)value, 0);
// } else {
//   new = smd_attr_new_usertype(name, type_nc_to_esdm(type), etype, value, 0);
//   if(!new){
//     return NC_ERANGE;
//   }
// }

int main(int argc, char **argv) {
  int ncid, dimids[NUM_DIMS];
  int varid, cmode = 0;
  int dimid[NDIMS], var_dimids[VAR_DIMS] = {2, 1, 0};
  char long_name[] = PRES_MAX_WIND;

  if (nc_create(FILE_NAME, cmode, &ncid)) ERR;
  if (nc_def_dim(ncid, DIM_A, DIM_A_LEN, &dimid[0])) ERR;
  if (nc_def_dim(ncid, DIM_B, DIM_B_LEN, &dimid[1])) ERR;
  if (nc_def_dim(ncid, DIM_C, DIM_C_LEN, &dimid[2])) ERR;
  if (nc_def_var(ncid, CXX_VAR_NAME, NC_FLOAT, VAR_DIMS, var_dimids, &varid)) ERR;
  if (varid) ERR;
  if (nc_put_att(ncid, varid, LONG_NAME, NC_CHAR, strlen(long_name) + 1, long_name)) ERR;
  if (nc_put_att(ncid, varid, UNITS, NC_CHAR, strlen(UNITS) + 1, UNITS)) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
