#include <config.h>
#include <nc_tests.h>
#include <nc4internal.h>
#include "err_macros.h"

#define FILE_NAME "tst_dims.nc"
#define LAT_NAME "lat"
#define LAT_LEN 1
#define MAX_DIMS 5

// Changes in ESDM__enddef are generating a problem here.

int
main(int argc, char **argv)
{

     int ncid, dimid, dimid2;
     int ndims_in, dimids_in[MAX_DIMS];
     size_t len_in;
     char name_in[NC_MAX_NAME + 1];
     int dimid_in;

     /* Create a file with one dim and nothing else. */
     if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

     if (nc_enddef(ncid)) ERR;

     /* Create the dim. */
     if (nc_def_dim(ncid, LAT_NAME, LAT_LEN, &dimid)) ERR;
     if (nc_close(ncid)) ERR;

     /* Reopen and check it out again. */
     if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
     if (nc_inq_dim(ncid, dimid, name_in, &len_in)) ERR;
     if (len_in != LAT_LEN || strcmp(name_in, LAT_NAME)) ERR;
     if (nc_close(ncid)) ERR;

   SUMMARIZE_ERR;
   FINAL_RESULTS;
}
