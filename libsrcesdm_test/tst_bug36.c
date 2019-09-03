/* This is part of the netCDF package. Copyright 2009 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use.

   Test netcdf-4 coordinate variables and dimensions.

   $Id: tst_coords.c,v 1.20 2010/03/30 16:25:41 ed Exp $
*/

#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"

#define INST_NAME "institution"
#define INSTITUTION "NCAR (National Center for Atmospheric \nResearch, Boulder, CO, USA)"
#define FILE_NC4 "tst_coords_nc4_att.nc"

int
main(int argc, char **argv)
{
    int ncid_nc4;
    /*int attid;*/
    char att_in_classic[NC_MAX_NAME + 1], att_in_nc4[NC_MAX_NAME + 1];

    if (nc_create(FILE_NC4, NC_NETCDF4, &ncid_nc4)) ERR;
    if (nc_put_att_text(ncid_nc4, NC_GLOBAL, INST_NAME, strlen(INSTITUTION) + 1, INSTITUTION)) ERR;
    if (nc_close(ncid_nc4)) ERR;

    if (nc_open(FILE_NC4, 0, &ncid_nc4)) ERR;
    if (nc_get_att_text(ncid_nc4, NC_GLOBAL, INST_NAME, att_in_nc4)) ERR;
    if (strcmp(att_in_nc4, INSTITUTION)) ERR;
    if (nc_close(ncid_nc4)) ERR;

   FINAL_RESULTS;
}
