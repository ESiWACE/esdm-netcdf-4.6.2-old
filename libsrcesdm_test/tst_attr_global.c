#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>

#include "netcdf.h"

/* This is the name of the data file we will create. */
// #define FILE_NAME "esdm://tst_select.dat"
// #define FILE_NAME "tst_select.esdm"
#define FILE_NAME "testfile.nc4"
#define TEST_FLAGS NC_NETCDF4

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(ERRCODE);}

static void write(){
  int ncid;
  int retval;
  if ((retval = nc_create(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
     ERR(retval);

  short s = 32;
  const char * str = "this is test1";
  if ((retval = nc_put_att_short(ncid, NC_GLOBAL, "short", NC_SHORT, 1, & s))) ERR(retval);

  if ((retval = nc_enddef(ncid)))
     ERR(retval);

  if ((retval = nc_close(ncid)))
    ERR(retval);
}

static void read(){
  int ncid;
  int retval;
  if ((retval = nc_open(FILE_NAME, NC_NOCLOBBER | TEST_FLAGS, & ncid)))
     ERR(retval);
  short s = 1;
  if ((retval = nc_get_att_short(ncid, NC_GLOBAL, "short", & s))) ERR(retval);
  assert( s == 32 );

  if ((retval = nc_close(ncid)))
     ERR(retval);
}

int main (int argc, char ** argv){
  write();
  printf("OK\n");
  return 0;
}
