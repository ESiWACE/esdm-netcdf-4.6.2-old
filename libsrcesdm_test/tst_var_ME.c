#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>

#include "netcdf.h"

#define FILE_NAME "tst_var_ME.nc4"
#define TEST_FLAGS NC_ESDM

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e)                                          \
 {                                                     \
   printf("Error: %d %s\n", __LINE__, nc_strerror(e)); \
   exit(ERRCODE);                                      \
 }

static void write_test();
static void read_test();

int main(int argc, char **argv) {
  write_test();
  read_test();

  nc_finalize();

  printf("*** SUCCESS attributes!\n");
  return 0;
}

static void write_test() {
  int ncid, x1_dimid, x2_dimid, x3_dimid;
  int *dimidsp, ndims_var, varid;

  int x, y, retval;

  if ((retval = nc_create(FILE_NAME, NC_NOWRITE | TEST_FLAGS, &ncid)))
    ERR(retval);

  if ((retval = nc_def_dim(ncid, "z", 20, &x3_dimid)))
    ERR(retval);

  ndims_var = 1;
  dimidsp = malloc(sizeof(int) * ndims_var);
  dimidsp[0] = x3_dimid;

  if ((retval = nc_def_var(ncid, "temp", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  const char *str1 = "atmosphere_sigma_coordinate";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str1))) ERR(retval);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if (retval = nc_close(ncid))
    ERR(retval);
}

static void read_test() {
  int ncid, varid, retval;

  if ((retval = nc_open(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
    ERR(retval);

  // char *name;
  nc_type xtypep;
  int ndimsp;
  // int *dimidsp;
  int nattsp;

  // if ((retval = NC_inq_var_all(ncid, varid, name))) ERR(retval);

  varid = 0;
  // if ((retval = NC_inq_var_all(ncid, varid, name, &xtypep, &ndimsp, dimidsp, &nattsp))) ERR(retval);
  // // assert(strcmp(name, "temp") == 0);
  // assert(xtypep == NC_DOUBLE);
  // assert(ndimsp == 1);
  // assert(nattsp == 1);

  char name[255];
  int dimidsp[10];

  if ((retval = NC_inq_var_all(ncid, varid, name, &xtypep, &ndimsp, dimidsp, &nattsp, NULL , NULL , NULL , NULL , NULL , NULL , NULL , NULL , NULL , NULL , NULL , NULL))) ERR(retval);
  assert(strcmp(name, "temp") == 0);
  assert(xtypep == NC_DOUBLE);
  assert(ndimsp == 1);
  assert(dimidsp[0] == 0);
  assert(nattsp == 1);

  if ((retval = nc_enddef(ncid))) ERR(retval);
  if ((retval = nc_close(ncid))) ERR(retval);
}
