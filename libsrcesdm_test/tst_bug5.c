#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include "netcdf_f.h"

#define FILE_NAME "tst_bug5.nc"
#define VAR_NAME7 "V5"

// Another problem with ulonglong

int main(int argc, char **argv) {
  int ncid, varid;

  unsigned long long data = 9223372036854775817ull, data_in;

  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_var(ncid, VAR_NAME7, NC_UINT64, 0, NULL, &varid)) ERR;
  if (nc_put_var_ulonglong(ncid, varid, &data)) ERR;
  if (nc_get_var_ulonglong(ncid, varid, &data_in)) ERR;
  if (data_in != data) ERR;
  if (nc_close(ncid)) ERR;

  if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
  if (nc_close(ncid)) ERR;

  FINAL_RESULTS;
}
