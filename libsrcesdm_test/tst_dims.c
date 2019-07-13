#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>

#include "netcdf.h"

// This program is derived from simple_xy_wr.c

/* This is the name of the data file we will create. */
// #define FILE_NAME "esdm://tst_select.dat"
// #define FILE_NAME "tst_select.esdm"
#define FILE_NAME "tst_dims.nc4"
#define TEST_FLAGS NC_ESDM
#define NC_DIMS 4

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e)                             \
  {                                        \
    printf("Error: %s\n", nc_strerror(e)); \
    exit(ERRCODE);                         \
  }

static void write_test ();
static void read_test ();

int main(int argc, char **argv) {
  /* When we create netCDF variables and dimensions, we get back an
    * ID for each one. */
    write_test ();
    // read_test ();

    printf("*** SUCCESS attributes!\n");
    return 0;
}

static void write_test ()
{
  int ncid, x1_dimid, x2_dimid, x3_dimid, x4_dimid, varid, ndims_var;
  int *dimidsp;
  /* Loop indexes, and error handling. */
  int x, y, retval;
  // if ((retval = nc_create(FILE_NAME, NC_PERSIST | NC_ESDM, &ncid)))
  if ((retval = nc_create(FILE_NAME, NC_NOWRITE | TEST_FLAGS, &ncid)))
    ERR(retval);

// This line is an enigma.
// Without NC_ESDM it makes the file persistent, but nc_put_att_string doesn't work.
// With NC_ESDM nc_put_att_string works.

  /* Define the dimensions. NetCDF will hand back an ID for each. */
  /* Define the dimensions. NetCDF will hand back an ID for each. */
  if ((retval = nc_def_dim(ncid, "x", 106, &x1_dimid)))
     ERR(retval);
  if ((retval = nc_def_dim(ncid, "y", 110, &x2_dimid)))
     ERR(retval);
  if ((retval = nc_def_dim(ncid, "z", 20, &x3_dimid)))
     ERR(retval);
  if ((retval = nc_def_dim(ncid, "bounds", 2, &x4_dimid)))
     ERR(retval);

  /* The dimids array is used to pass the IDs of the dimensions of
    * the variable. */
  // dimids[0] = x1_dimid;
  // dimids[1] = x2_dimid;
  // dimids[2] = x3_dimid;
  // dimids[3] = x4_dimid;

  // Define the variable.

//     double t ;                                        // Domain axis and dimension coordinate construct  8
//         t:standard_name = "time" ;                                                                   //  9
//         t:units = "days since 2016­12­01" ;                                                          // 10

  ndims_var = 0;
  dimidsp = NULL;
  if ((retval = nc_def_var(ncid, "t", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  const char * str1 = "time";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str1))) ERR(retval);
  // nc_free_string(1, (char **)&str1);

  const char * str2 = "days since 2016­12­01";
  if ((retval = nc_put_att_string(ncid, varid, "units", 1, &str2))) ERR(retval);
  // nc_free_string(1, (char **)&str2);

//     double z(z) ;                                // Dimension coordinate and domain ancillary construct 13
//         z:standard_name = "atmosphere_sigma_coordinate" ;                                            // 14
//         z:positive = "down" ;                                                                        // 15

  ndims_var = 1;
  dimidsp = malloc(sizeof(int)*ndims_var);
  dimidsp[0] = x3_dimid;

  if ((retval = nc_def_var(ncid, "z", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  const char * str3 = "atmosphere_sigma_coordinate";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str3))) ERR(retval);
  // nc_free_string(1, (char **)&str3);

  const char * str4 = "down";
  if ((retval = nc_put_att_string(ncid, varid, "positive", 1, &str4))) ERR(retval);
  // nc_free_string(1, (char **)&str4);

//  double lon(y, x) ;                                                // Auxiliary coordinate construct 27
//         lon:standard_name = "longitude" ;                                                            // 28
//         lon:units = "degrees_east" ;                                                                 // 29

  ndims_var = 2;
  dimidsp = malloc(sizeof(int)*ndims_var);
  dimidsp[0] = x2_dimid;
  dimidsp[1] = x1_dimid;

  if ((retval = nc_def_var(ncid, "lon", NC_DOUBLE, ndims_var, dimidsp, &varid))) ERR(retval);

  const char * str5 = "atmosphere_sigma_coordinate";
  if ((retval = nc_put_att_string(ncid, varid, "stardard_name", 1, &str3))) ERR(retval);
  // nc_free_string(1, (char **)&str3);

  const char * str6 = "longitude";
  if ((retval = nc_put_att_string(ncid, varid, "units", 1, &str4))) ERR(retval);
  // nc_free_string(1, (char **)&str4);

  if ((retval = nc_enddef(ncid)))
     ERR(retval);

  if (retval = nc_close(ncid))
    ERR(retval);
}

static void read_test ()
{
  int ncid, varid, retval;

  if ((retval = nc_open(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
    ERR(retval);

  // nc_inq_varid (ncid, "data", &varid);
  // printf("\n\nvarid = %d", varid);
  varid = 0;

  char * str_new;

  if ((retval = nc_get_att_string(ncid, varid, "string", & str_new))) ERR(retval);
  assert(strcmp("this is test1", str_new) == 0);

  // printf("\n\nstr_new = %s", str_new);

  nc_free_string(1, & str_new);

  if ((retval = nc_enddef(ncid)))
     ERR(retval);

  if ((retval = nc_close(ncid)))
    ERR(retval);
}
