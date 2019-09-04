#include "nc_tests.h"
#include "err_macros.h"
#include "netcdf.h"

#define FILE_NAME "tst_vars.nc"
#define DIM7_LEN 2
#define DIM7_NAME "dim_7_from_Indiana"
#define VAR7_NAME "var_7_from_Idaho"
#define NDIMS 1

int main(int argc, char **argv) {
  int ncid, dimids[NDIMS];
  int i, j;

  size_t index[NDIMS];
  int varid;
  int no_fill;
  unsigned short ushort_data = 42, ushort_data_in, fill_value_in;
  unsigned short my_fill_value = 999;

  /* Create a netcdf-4 file with one dim and 1 NC_USHORT var. */
  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_dim(ncid, DIM7_NAME, DIM7_LEN, &dimids[0])) ERR;

  if (nc_def_var(ncid, VAR7_NAME, NC_USHORT, NDIMS, dimids, &varid)) ERR;

  if (nc_put_att_ushort(ncid, varid, "_FillValue", NC_USHORT, 1, & ushort_data)) ERR;
  fill_value_in = -1;
  if (nc_get_att_ushort(ncid, varid, "_FillValue", & fill_value_in)) ERR;
  assert(ushort_data == fill_value_in);
  fill_value_in = -1;

  /* Turn off fill mode. */
  if (nc_def_var_fill(ncid, varid, 1, NULL)) ERR;
  if (nc_inq_var_fill(ncid, varid, &no_fill, &fill_value_in)) ERR;
  if (!no_fill) ERR;

  /* Turn on fill mode. */

  if (nc_def_var_fill(ncid, varid, 0, & ushort_data)) ERR;
  if (nc_inq_var_fill(ncid, varid, &no_fill, &fill_value_in)) ERR;
  if (no_fill) ERR;

  /* Turn off fill mode. */
  if (nc_def_var_fill(ncid, varid, 1, NULL)) ERR;
  if (nc_inq_var_fill(ncid, varid, &no_fill, &fill_value_in)) ERR;
  if (!no_fill) ERR;

  /* Try and set a fill value and fill mode off. It will be
       * ignored because fill mode is off. */
  if (nc_def_var_fill(ncid, varid, 1, &my_fill_value)) ERR;

  /* Turn on fill mode. */
  if (nc_def_var_fill(ncid, varid, 0, NULL)) ERR;
  if (nc_inq_var_fill(ncid, varid, &no_fill, &fill_value_in)) ERR;
  if (fill_value_in != NC_FILL_USHORT) ERR;
  if (no_fill) ERR;

  /* Write the second of two values. */
  index[0] = 1;
  if (nc_put_var1_ushort(ncid, varid, index, &ushort_data)) ERR;

  /* Get the first value, and make sure we get the default fill
       * value for USHORT. */
  index[0] = 0;
  if (nc_get_var1_ushort(ncid, varid, index, &ushort_data_in)) ERR;
  if (ushort_data_in != NC_FILL_USHORT) ERR;
  if (nc_close(ncid)) ERR;

  /* Open the file and check the same stuff. */
  if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;

  /* Check stuff. */
  if (nc_inq_var_fill(ncid, varid, &no_fill, &fill_value_in)) ERR;
  if (no_fill) ERR;

  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
