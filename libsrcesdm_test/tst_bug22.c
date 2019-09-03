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
#define ERR                                \
  {                                        \
    printf("Error!\n"); \
    exit(ERRCODE);                         \
  }

#define INST_NAME "institution"
#define INSTITUTION "NCAR (National Center for Atmospheric \nResearch, Boulder, CO, USA)"
#define FILE_NC4 "tst_coords_nc4_att.nc"

int main(int argc, char **argv) {

  nc_initialize();

  int ncid_nc4;
  char att_in_nc4[NC_MAX_NAME + 1];

  if (nc_create(FILE_NC4, NC_NETCDF4, &ncid_nc4)) ERR;

  if (nc_put_att_text(ncid_nc4, NC_GLOBAL, INST_NAME, strlen(INSTITUTION) + 1, INSTITUTION)) ERR;

  if (nc_close(ncid_nc4)) ERR;

  if (nc_open(FILE_NC4, 0, &ncid_nc4)) ERR;

  if (nc_get_att_text(ncid_nc4, NC_GLOBAL, INST_NAME, att_in_nc4)) ERR;

  if (strcmp(att_in_nc4, INSTITUTION)) printf("\nHere we have a problem!\n\n");

  if (nc_close(ncid_nc4)) ERR;

  nc_finalize();

  printf("*** SUCCESS attributes!\n");
  return 0;
}
