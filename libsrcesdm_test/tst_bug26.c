/* This is part of the netCDF package. Copyright 2009 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use.

   Test netcdf-4 coordinate variables and dimensions.

   $Id: tst_coords.c,v 1.20 2010/03/30 16:25:41 ed Exp $
*/

#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_coords.nc"
#define VAR_NAME "Britany"
#define NDIMS 2
#define NDIMS_1 1
#define WINSTON_CHURCHILL "Winston_S_Churchill"
#define D0_LEN 2
#define NUM_VARS_2 2

int
main(int argc, char **argv)
{

   printf("**** testing new order of doing things with coordinate variable...");
   {
      /* In this test:
           define a dimension
           define a variable that uses that dimension
           put values in the variable
           define coordinate values for the dimension
      */
      int ncid, nvars_in, varids_in[NUM_VARS_2];
      int dimid, varid, varid2;
      int nvars, ndims, ngatts, unlimdimid;
      int ndims_in, natts_in, dimids_in[NDIMS];
      char var_name_in[NC_MAX_NAME + 1];
      nc_type xtype_in;
      int data[D0_LEN] = {42, -42};

      /* Create a netcdf-4 file with 2D coordinate var. */
      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_def_dim(ncid, WINSTON_CHURCHILL, NC_UNLIMITED, &dimid)) ERR;
      if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIMS_1, &dimid, &varid)) ERR;
      // if (nc_put_var_int(ncid, varid, data)) ERR;
      // if (nc_close(ncid)) ERR;

   }
   FINAL_RESULTS;
}
