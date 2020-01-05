#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "netcdf.h"

// This program is derived from simple_xy_wr.c

/* This is the name of the data file we will create. */
// #define FILE_NAME "esdm://tst_select.dat"
// #define FILE_NAME "tst_select.esdm"
#define FILE_NAME "tst.nc4"

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e)                             \
  {                                        \
    printf("Error: %s\n", nc_strerror(e)); \
    exit(ERRCODE);                         \
  }

int main(int argc, char **argv) {
  /* When we create netCDF variables and dimensions, we get back an
    * ID for each one. */
  int ncid, x_dimid, y_dimid, varid;
  int dimids[2];
  /* Loop indexes, and error handling. */
  int x, y, retval;
  // if ((retval = nc_create(FILE_NAME, NC_CLOBBER | NC_NETCDF4, &ncid)))
  if ((retval = nc_create(FILE_NAME, NC_CLOBBER | NC_ESDM, &ncid)))
    ERR(retval);

  /* Define the dimensions. NetCDF will hand back an ID for each. */
  if ((retval = nc_def_dim(ncid, "x", 2, &x_dimid)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "y", 2, &y_dimid)))
    ERR(retval);

  /* The dimids array is used to pass the IDs of the dimensions of
    * the variable. */
  dimids[0] = x_dimid;
  dimids[1] = y_dimid;

  /* Define the variable. The type of the variable in this case is
    * NC_INT (4-byte integer). */
  if ((retval = nc_def_var(ncid, "data", NC_INT, 2, dimids, &varid))) ERR(retval);

  short s = 32;
  const char *str = "this is test1";
  if ((retval = nc_put_att_short(ncid, NC_GLOBAL, "short", NC_SHORT, 1, &s))) ERR(retval);
  if ((retval = nc_put_att_string(ncid, varid, "string", 1, &str))) ERR(retval);
  s = 1;

  if ((retval = nc_get_att_short(ncid, NC_GLOBAL, "short", &s))) ERR(retval);
  assert(s == 32);

  char *str_new;
  if ((retval = nc_get_att_string(ncid, varid, "string", &str_new))) ERR(retval);
  assert(strcmp(str, str_new) == 0);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if ((retval = nc_close(ncid)))
    ERR(retval);

  nc_free_string(1, &str_new);

  nc_finalize();

  printf("*** SUCCESS attributes!\n");
  return 0;
}
