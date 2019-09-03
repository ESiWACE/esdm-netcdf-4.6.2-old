#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "netcdf.h"

/* This is the name of the data file we will create. */
// #define FILE_NAME "esdm://tst_select.dat"
// #define FILE_NAME "tst_select.esdm"
#define FILE_NAME "tst_global.nc4"
#define TEST_FLAGS NC_ESDM
//NC_NETCDF4

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e)                             \
  {                                        \
    printf("Error: %s\n", nc_strerror(e)); \
    exit(ERRCODE);                         \
  }

static void write_test();
static void read_test();

int main(int argc, char **argv) {
  nc_initialize();

  write_test();
  read_test();
  // printf("OK\n");

  nc_finalize();
  printf("*** SUCCESS attributes!\n");
  return 0;
}

static void write_test() {
  int ncid;
  int retval;
  if ((retval = nc_create(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
    ERR(retval);

  short s = 32;
  if ((retval = nc_put_att_short(ncid, NC_GLOBAL, "short", NC_SHORT, 1, &s))) ERR(retval);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if ((retval = nc_close(ncid)))
    ERR(retval);
}

static void read_test() {
  int ncid;
  int retval;
  if ((retval = nc_open(FILE_NAME, NC_NOWRITE | TEST_FLAGS, &ncid)))
    ERR(retval);
  short s = 1;
  if ((retval = nc_get_att_short(ncid, NC_GLOBAL, "short", &s))) ERR(retval);
  assert(s == 32);

  if ((retval = nc_close(ncid)))
    ERR(retval);
}
