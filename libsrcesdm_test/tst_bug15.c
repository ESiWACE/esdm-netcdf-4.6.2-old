#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include <signal.h>

#define FILE_NAME "tst_bug15.nc"
#define ATT_INT_NAME "Old_Bailey_Room_Numbers"
#define ATT_LEN 3

// Expected NC_ERANGE error but does the conversion anyway?
// (gdb) p uint_in
// $1 = {0, 128, 1431652752}
// (gdb) p uint_out
// $2 = {0, 128, 4294967295}
// (gdb) p NC_MAX_UINT
// $3 = 4294967295

int
main(int argc, char **argv)
{
    unsigned int uint_in[ATT_LEN], uint_out[ATT_LEN] = {0, 128, NC_MAX_UINT};

    int ncid, i;

    if (nc_create(FILE_NAME, NC_NETCDF4|NC_CLASSIC_MODEL, &ncid)) ERR;
    if (nc_put_att_uint(ncid, NC_GLOBAL, ATT_INT_NAME, NC_INT, ATT_LEN, uint_out)) ERR;
    // if (nc_put_att_uint(ncid, NC_GLOBAL, ATT_INT_NAME, NC_INT, ATT_LEN, uint_out) != NC_ERANGE) ERR;
    if (nc_get_att_uint(ncid, NC_GLOBAL, ATT_INT_NAME, uint_in)) ERR;
    // if (nc_get_att_uint(ncid, NC_GLOBAL, ATT_INT_NAME, uint_in) != NC_ERANGE) ERR;
    for (i = 0; i < ATT_LEN; i++)
      if (uint_in[i] != uint_out[i]) ERR;
    if (nc_close(ncid)) ERR;

    FINAL_RESULTS;
}
