/* This is part of the netCDF package. Copyright 2005-2018 University
   Corporation for Atmospheric Research/Unidata. See COPYRIGHT file
   for conditions of use.

   Test copy of attributes.

   Ed Hartnett, Denis Heimbigner, Ward Fisher
*/

#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME1 "tst_atts2.nc"
#define FILE_NAME2 "tst_atts2_2.nc"
#define ATT_NAME "Irish_Leader"
#define ATT_LEN 1

// Problem to copy the attribute string.

int
main(int argc, char **argv)
{

      int ncid1, ncid2;
      const char *mc[ATT_LEN] = {"Michael Collins"};
      char *mc_in;

      /* Create a file with a string att. */
      if (nc_create(FILE_NAME1, NC_NETCDF4, &ncid1)) ERR;
      if (nc_put_att_string(ncid1, NC_GLOBAL, ATT_NAME, ATT_LEN, mc)) ERR;
      if (nc_create(FILE_NAME2, NC_NETCDF4, &ncid2)) ERR;
      if (nc_copy_att(ncid1, NC_GLOBAL, ATT_NAME, ncid2, NC_GLOBAL)) ERR;
      if (nc_close(ncid1)) ERR;
      if (nc_close(ncid2)) ERR;

      /* Reopen file 2 and see that attribute is correct. */
      if (nc_open(FILE_NAME2, 0, &ncid2)) ERR;
      if (nc_get_att_string(ncid2, NC_GLOBAL, ATT_NAME, &mc_in)) ERR;
      if (strcmp(mc[0], mc_in)) ERR;
      if (nc_close(ncid2)) ERR;
      if (nc_free_string(ATT_LEN, &mc_in)) ERR;

   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
