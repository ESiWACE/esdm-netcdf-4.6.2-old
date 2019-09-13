#include <nc_tests.h>
#include "err_macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <netcdf.h>

#define FILE_NAME "tst_fills2.nc"
#undef STRING_VAR_NAME
#define STRING_VAR_NAME "I_Have_A_Dream"
#undef NDIMS_STRING
#define NDIMS_STRING 1
#define FILLVALUE_LEN 1 /* There is 1 string, the empty one. */
#define DATA_START 1

int main(int argc, char **argv) {
  int ncid, varid, dimid, varid_in;
  const char *missing_val[FILLVALUE_LEN] = {""};
  const char *missing_val_in[FILLVALUE_LEN];
  size_t index = DATA_START;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, "sentence", NC_UNLIMITED, &dimid)) ERR;
  if (nc_def_var(ncid, STRING_VAR_NAME, NC_STRING, NDIMS_STRING, &dimid, &varid)) ERR;
  if (nc_put_att_string(ncid, varid, "_FillValue", FILLVALUE_LEN, missing_val)) ERR;

  /* Check it out. */
  if (nc_inq_varid(ncid, STRING_VAR_NAME, &varid_in)) ERR;
  if (nc_get_att_string(ncid, varid_in, "_FillValue", (char **)missing_val_in)) ERR;
  if (strcmp(missing_val[0], missing_val_in[0])) ERR;
  if (nc_free_string(FILLVALUE_LEN, (char **)missing_val_in)) ERR;
  if (nc_close(ncid)) ERR;

  /* Now re-open file, read data, and check values again. */
  if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
  if (nc_inq_varid(ncid, STRING_VAR_NAME, &varid_in)) ERR;
  if (nc_get_att_string(ncid, varid_in, "_FillValue", (char **)missing_val_in)) ERR;
  if (strcmp(missing_val[0], missing_val_in[0])) ERR;
  if (nc_free_string(FILLVALUE_LEN, (char **)missing_val_in)) ERR;

  FINAL_RESULTS;
}
