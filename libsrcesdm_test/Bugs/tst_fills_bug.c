#include <nc_tests.h>
#include "err_macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define FILE_NAME "tst_fills.nc"
#define MAX_VALS 10

// tst_fills_bug: /home/lucy/esiwace/esdm/src/esdm-scheduler.c:795: fragmentsCoverSpace: Assertion `fragments' failed.

int main(int argc, char **argv)
{
      int  ncid, rec_id, dims[1];
      static int rec[1] = {4711};
      size_t start[1] = {0};
      size_t count[1] = {1};
      size_t count_get[1] = {MAX_VALS};
      int vals[MAX_VALS];
      memset(vals, -1, sizeof(vals));
      int i;

      if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

      /* Define dimensions and two vars, a 1D coordinate var for
       * unlimited dimension, and a 2D var which uses the unlimited
       * dimension. */
      if (nc_def_dim(ncid, "rec", NC_UNLIMITED, &dims[0])) ERR;
      if (nc_def_var(ncid, "rec", NC_INT, 1, dims, &rec_id)) ERR;

      /* Extend record dimension by 1. */
      if (nc_put_vara_int(ncid, rec_id, start, count, rec)) ERR;

      /* Read the other variable; it must have only fill values. */
      if (nc_get_vara_int(ncid, rec_id, start, count_get, vals)) ERR;
      if (nc_close(ncid)) ERR;

      assert(vals[0] == 4711);
      for(int i=1; i < MAX_VALS; i++){
          printf("%d\n", vals[i]);
          assert(vals[i] == -2147483647);
      }
      printf("[OK]\n");
      return 0;
}
