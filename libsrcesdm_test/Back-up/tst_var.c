#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>

#include "netcdf.h"

// This program is derived from simple_xy_wr.c

/* This is the name of the data file we will create. */
// #define FILE_NAME "esdm://tst_select.dat"
// #define FILE_NAME "tst_select.esdm"
#define FILE_NAME "tst_var.nc4"
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
  nc_initialize();
  /* When we create netCDF variables and dimensions, we get back an
    * ID for each one. */
  write_test();
  read_test();

  nc_finalize();

  printf("*** SUCCESS attributes!\n");
  return 0;
}

static void write_test() {
  int ncid, x1_dimid, x2_dimid, x3_dimid, x4_dimid, varid, ndims_var;
  int *dimidsp;
  /* Loop indexes, and error handling. */
  int x, y, retval;
  // if ((retval = nc_create(FILE_NAME, NC_PERSIST | NC_ESDM, &ncid)))
  if ((retval = nc_create(FILE_NAME, NC_NOWRITE | TEST_FLAGS, &ncid)))
    ERR(retval);

  /* Define the dimensions. NetCDF will hand back an ID for each. */

  if ((retval = nc_def_dim(ncid, "x", 106, &x1_dimid)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "y", 110, &x2_dimid)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "z", 20, &x3_dimid)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "bounds", 2, &x4_dimid)))
    ERR(retval);

  short s = 32;
  if ((retval = nc_put_att_short(ncid, NC_GLOBAL, "short", NC_SHORT, 1, &s))) ERR(retval);

  ndims_var = 1;
  dimidsp = malloc(sizeof(int) * ndims_var);
  dimidsp[0] = x3_dimid;

  if ((retval = nc_def_var(ncid, "z", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  if ((retval = nc_rename_var(ncid, varid, "new_z"))) ERR(retval);

  const char *str1 = "atmosphere_sigma_coordinate";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str1))) ERR(retval);
  // free(str2);

  const char *str2 = "down";
  if ((retval = nc_put_att_string(ncid, varid, "positive", 1, &str2))) ERR(retval);

  ndims_var = 2;
  dimidsp = malloc(sizeof(int) * ndims_var);
  dimidsp[0] = x2_dimid;
  dimidsp[1] = x1_dimid;

  if ((retval = nc_def_var(ncid, "lon", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  const char *str3 = "longitude";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str3))) ERR(retval);

  const char *str4 = "degrees_east";
  if ((retval = nc_put_att_string(ncid, varid, "units", 1, &str4))) ERR(retval);

  if ((retval = nc_rename_var(ncid, varid, "new_lon"))) ERR(retval);

  ndims_var = 3;
  dimidsp = malloc(sizeof(int) * ndims_var);
  dimidsp[0] = x3_dimid;
  dimidsp[1] = x2_dimid;
  dimidsp[2] = x1_dimid;

  if ((retval = nc_def_var(ncid, "temp", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  // Metadata doesn't show the name of the variable inside the dataset!!

  double v = 1.0e30;
  if ((retval = nc_put_att_double(ncid, varid, "missing_value", NC_DOUBLE, 1, &v))) ERR(retval);

  const char *str5 = "air_temperature";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str5))) ERR(retval);

  // nc_free_string(1, (char **)&str);
  // nc_free_string(3, (char **)names);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if (retval = nc_close(ncid))
    ERR(retval);
}

static void read_test() {
  int ncid, varid, retval;

  if ((retval = nc_open(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
    ERR(retval);

  if ((retval = nc_inq_varid(ncid, "temp", &varid))) ERR(retval);
  char *str_new;
  if ((retval = nc_get_att_string(ncid, varid, "stardard_name", &str_new))) ERR(retval);
  assert(strcmp(str_new, "air_temperature") == 0);
  if ((retval = nc_free_string(1, &str_new))) ERR(retval);

  if ((retval = nc_inq_varid(ncid, "new_z", &varid))) ERR(retval);
  if ((retval = nc_get_att_string(ncid, varid, "positive", &str_new))) ERR(retval);

  if ((retval = nc_inq_varid(ncid, "temp", &varid))) ERR(retval);
  double v = 1.0;
  if ((retval = nc_get_att_double(ncid, varid, "missing_value", &v))) ERR(retval);
  assert(v == 1.0e30);

  char name[255];
  nc_type xtypep;
  int ndimsp;
  int dimidsp[10];
  int nattsp;

  // Check the definitions when implementing the function

  int shufflep;
  int deflatep;
  int deflate_levelp;
  int fletcher32p;
  int contiguousp;
  size_t chunksizesp;
  int no_fill;
  void *fill_valuep;
  int endiannessp;
  unsigned int idp;
  size_t nparamsp;
  unsigned int params;

  if ((retval = NC_inq_var_all(ncid, varid, name, &xtypep, &ndimsp, dimidsp, &nattsp, &shufflep, &deflatep, &deflate_levelp, &fletcher32p, &contiguousp, &chunksizesp, &no_fill, &fill_valuep, &endiannessp, &idp, &nparamsp, &params))) ERR(retval);

  assert(strcmp(name, "temp") == 0);
  assert(xtypep == NC_DOUBLE);
  assert(ndimsp == 3);
  assert(dimidsp[0] == 2);
  assert(dimidsp[1] == 1);
  assert(dimidsp[2] == 0);
  assert(nattsp == 2);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if ((retval = nc_close(ncid)))
    ERR(retval);
}
