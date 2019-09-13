#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include <signal.h>

#define FILE_NAME "tst_atts1.nc"
#define ATT_SHORT_NAME "Ecclesiastical_Court_Appearences"
#define ATT_LEN 3

// (gdb) p double_in
// $1 = {0, 4.6355535805467796e-310, 6.9533558074294752e-310}
// (gdb) p short_out
// $2 = {-32768, -128, 32767}

int
main(int argc, char **argv)
{

    short short_in[ATT_LEN], short_out[ATT_LEN] = {NC_MIN_SHORT, -128, NC_MAX_SHORT};
    double double_in[ATT_LEN];

      int ncid, i;

      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
      if (nc_put_att_short(ncid, NC_GLOBAL, ATT_SHORT_NAME, NC_SHORT, ATT_LEN, short_out)) ERR;
      if (nc_close(ncid)) ERR;

      if (nc_open(FILE_NAME, 0, &ncid)) ERR;
      if (nc_get_att_double(ncid, NC_GLOBAL, ATT_SHORT_NAME, double_in)) ERR;
      for (i = 0; i < ATT_LEN; i++)
	     if (double_in[i] != short_out[i]) ERR;
      if (nc_close(ncid)) ERR;

   FINAL_RESULTS;
}
