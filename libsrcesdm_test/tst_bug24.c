/* This is part of the netCDF package. Copyright 2006-2018 University
   Corporation for Atmospheric Research/Unidata. See COPYRIGHT file
   for conditions of use.

   Test the netCDF-4 attribute code.

   Ed Hartnett
*/

#include <config.h>
#include <nc_tests.h>
#include "err_macros.h"
#include "hdf5internal.h"

/* The data file we will create. */
#define FILE_NAME "tst_atts.nc"

/* Contents of attributes. */
#define VAR_NAME "Earth"

int
main(int argc, char **argv)
{
      int ncid, attid, varid;

      /* Create a file with an att. */
      if (nc_create(FILE_NAME, NC_NETCDF4|NC_CLOBBER, &ncid)) ERR;
      if (nc_def_var(ncid, VAR_NAME, NC_INT, 0, NULL, &varid)) ERR;
      if (nc_close(ncid)) ERR;

   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
