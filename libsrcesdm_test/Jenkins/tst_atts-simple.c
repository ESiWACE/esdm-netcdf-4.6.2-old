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

/* Names of attributes. */
#define OLD_NAME_2 "Constantinopolis"

int main(int argc, char **argv)
{
      int ncid;
      int natts;

      /* Create a file with two atts. */
      if (nc_create(FILE_NAME, NC_NETCDF4|NC_CLOBBER, &ncid)) ERR;

      if (nc_put_att_text(ncid, NC_GLOBAL, OLD_NAME_2, 0, NULL)) ERR;

      /* End define mode. It redef will be called automatically. */
      if (nc_enddef(ncid)) ERR;

      /* Delete the attribute. */
      // if (nc_del_att(ncid, NC_GLOBAL, OLD_NAME)) ERR;
      if (nc_close(ncid)) ERR;

   SUMMARIZE_ERR;
   FINAL_RESULTS;

}
