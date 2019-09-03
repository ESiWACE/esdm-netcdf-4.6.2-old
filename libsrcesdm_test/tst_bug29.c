/* This is part of the netCDF package. Copyright 2008 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use. See www.unidata.ucar.edu for more info.

   Test netcdf-4 dimensions some more.

   Ed Hartnett
*/

#include <config.h>
#include <nc_tests.h>
#include "err_macros.h"

#define FILE_NAME "tst_dims2.nc"

#define NDIMS1 1
#define NDIMS2 2

int
main(int argc, char **argv)
{
   printf("\n*** Testing netcdf-4 dimensions some more.\n");
   printf("*** Checking non-coordinate variable with same name as dimension...");
   {
#define CRAZY "crazy"
#define NUTS "nuts"
#define NUM_CRAZY 3
#define NUM_NUTS 5
      int nuts_dimid, crazy_dimid, dimid_in;
      int varid, ncid;
      nc_type xtype_in, xtype = NC_CHAR;
      int ndims_in, nvars_in, natts_in, unlimdimid_in;
      char name_in[NC_MAX_NAME + 1];

      /* Create a file with 2 dims and one var. The var is named the
       * same as one of the dimensions, but uses the other dimension,
       * and thus is not a netCDF coordinate variable. */
      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_def_dim(ncid, CRAZY, NUM_CRAZY, &crazy_dimid)) ERR;
      if (nc_def_dim(ncid, NUTS, NUM_NUTS, &nuts_dimid)) ERR;
      if (nc_def_var(ncid, CRAZY, xtype, NDIMS1, &nuts_dimid, &varid)) ERR;

      /* Check out the file. */
      if (nc_inq(ncid, &ndims_in, &nvars_in, &natts_in, &unlimdimid_in)) ERR;
      if (ndims_in != 2 || nvars_in != 1 || natts_in != 0 || unlimdimid_in != -1) ERR;
      if (nc_inq_var(ncid, 0, name_in, &xtype_in, &ndims_in, &dimid_in, &natts_in)) ERR;
      if (strcmp(name_in, CRAZY) || xtype_in != xtype || ndims_in != NDIMS1 ||
          natts_in != 0 || dimid_in != nuts_dimid) ERR;

      if (nc_close(ncid)) ERR;

      /* Reopen and check the file. */
      if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;

      if (nc_inq(ncid, &ndims_in, &nvars_in, &natts_in, &unlimdimid_in)) ERR;
      if (ndims_in != 2 || nvars_in != 1 || natts_in != 0 || unlimdimid_in != -1) ERR;
      if (nc_inq_var(ncid, 0, name_in, &xtype_in, &ndims_in, &dimid_in, &natts_in)) ERR;
      if (strcmp(name_in, CRAZY) || xtype_in != xtype || ndims_in != NDIMS1 ||
          natts_in != 0 || dimid_in != nuts_dimid) ERR;

      if (nc_close(ncid)) ERR;

   }
   FINAL_RESULTS;
}
