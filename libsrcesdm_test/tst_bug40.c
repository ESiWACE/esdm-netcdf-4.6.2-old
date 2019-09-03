/* This is part of the netCDF package.
   Copyright 2005 University Corporation for Atmospheric Research/Unidata
   See COPYRIGHT file for conditions of use.

   Test netcdf-4 variables.
   Ed Hartnett, Russ Rew, Dennis Heimbigner, Ward Fisher
*/

#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_vars3.nc"
#define NDIMS1 1
#define NDIMS2 2
#define D_SMALL "small_dim"
#define D_SMALL_LEN 16
#define D_MEDIUM "medium_dim"
#define D_MEDIUM_LEN 65546
#define D_LARGE "large_dim"
#define D_LARGE_LEN 1048586
#define V_SMALL "small_var"
#define V_MEDIUM "medium_var"
#define V_LARGE "large_var"
#define D_MAX_ONE_D 16384

int
main(int argc, char **argv)
{

   printf("**** testing definition of coordinate variable with some data...");
   {
#define NX 6
#define NY 36
#define V1_NAME "V1"
#define D1_NAME "D1"
#define D2_NAME "D2"

      int ncid, x_dimid, y_dimid, varid1, varid2;
      int nvars, ndims, ngatts, unlimdimid, dimids_in[2], natts;
      size_t len_in;
      char name_in[NC_MAX_NAME + 1];
      nc_type xtype_in;

      /* Create file with two dims, two 1D vars. */
      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_def_dim(ncid, D1_NAME, NX, &x_dimid)) ERR;
      if (nc_def_dim(ncid, D2_NAME, NY, &y_dimid)) ERR;
      if (nc_def_var(ncid, V1_NAME, NC_DOUBLE, NDIMS1, &y_dimid, &varid1)) ERR;
      if (nc_enddef(ncid)) ERR;
      if (nc_redef(ncid)) ERR;
      if (nc_def_var(ncid, D1_NAME, NC_DOUBLE, NDIMS1, &x_dimid, &varid2)) ERR;

/*       if (nc_put_var_double(ncid, varid1, &data_outy[0])) ERR; */
/*       if (nc_put_var_double(ncid, varid2, &data_outx[0])) ERR; */
/*       if (nc_sync(ncid)) ERR; */

      /* Check the file. */
      if (nc_inq(ncid, &ndims, &nvars, &ngatts, &unlimdimid)) ERR;

      printf("\n*******************************************\n");
      printf("\nExpected values: nvars = %d || ndims = %d || ngatts = %d || unlimdimid = %d", 2, 2, 0, -1);
      printf("\nActual values: nvars = %d || ndims = %d || ngatts = %d || unlimdimid = %d", nvars, ndims, ngatts, unlimdimid);
      printf("\n\n_nc_dims and _nc_sizes are still there!\n");
      printf("\n*******************************************\n\n");

      if (nvars != 2 || ndims != 2 || ngatts != 0 || unlimdimid != -1) ERR;


      if (nc_close(ncid)) ERR;

   }
   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
