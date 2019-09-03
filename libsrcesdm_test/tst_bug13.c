#include "nc_tests.h"
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_vars.nc"
#define D1_NAME "unlimited"
#define D1_LEN 4
#define D1_TARGET 3
#define D2_NAME "fixed"
#define D2_LEN 3
#define D2_TARGET 2
#define V1_NAME "var1"
#define ND1 2
#define TARGET_VALUE 42

int main(int argc, char **argv) {
  int ncid, dimids[ND1], varid;
  int data = TARGET_VALUE, data_in[D1_LEN][D2_LEN];
  int i, j;
  size_t index[ND1];

  /* Create a netcdf-4 file with one dim and 1 NC_USHORT var. */
  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, D1_NAME, NC_UNLIMITED, &dimids[0])) ERR;
  if (nc_def_dim(ncid, D2_NAME, D2_LEN, &dimids[1])) ERR;
  if (nc_def_var(ncid, V1_NAME, NC_INT, ND1, dimids, &varid)) ERR;
  i = NC_FILL_INT;
  if (nc_def_var_fill(ncid, varid, 0, & i)) ERR;
  index[0] = D1_TARGET;
  index[1] = D2_TARGET;
  if (nc_put_var1_int(ncid, varid, index, &data)) ERR;

  /* Get the data, and check the values. */
  if (nc_get_var_int(ncid, varid, &data_in[0][0])) ERR;

      for (i = 0; i < D1_TARGET; i++)
   for (j = 0; j < D2_LEN; j++)
      if ((i == D1_TARGET && j == D2_TARGET && data_in[i][j] != TARGET_VALUE) ||
   data_in[i][j] != NC_FILL_INT) ERR;

     if (nc_close(ncid)) ERR;
  
     if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
  
     /* Get the data, and check the values. */
     if (nc_get_var_int(ncid, 0, &data_in[0][0])) ERR;
      for (i = 0; i < D1_TARGET; i++)
   for (j = 0; j < D2_LEN; j++)
      if ((i == D1_TARGET && j == D2_TARGET && data_in[i][j] != TARGET_VALUE) ||
   data_in[i][j] != NC_FILL_INT) ERR;

  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
