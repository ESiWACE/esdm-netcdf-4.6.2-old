#include <config.h>
#include <nc_tests.h>
#include "err_macros.h"

#define FILE_NAME "tst_bug48.nc"

// Problem with the conversion of vectors

int main(int argc, char **argv) {

    int dimids[2];
    int varid, ncid, timeDimID, beamDimID;
    int i, j;
    int value[2000];
    size_t start[] = {0, 0};
    size_t count[] = {1, 1};

    for (i = 0; i < 2000; i++)
      value[i] = 2000 - i;

    if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

    /* Define 2 unlimited dimensions */
    if (nc_def_dim(ncid, "time", NC_UNLIMITED, &timeDimID)) ERR;
    if (nc_def_dim(ncid, "beam", NC_UNLIMITED, &beamDimID)) ERR;

    dimids[0] = timeDimID;
    dimids[1] = beamDimID;

    if (nc_def_var(ncid, "depth", NC_DOUBLE, 2, dimids, &varid)) ERR;
    if (nc_put_vara_int(ncid, varid, start, count, value)) ERR;
    if (nc_close(ncid)) ERR;

    /* Check the file. */
    if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
    if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
