#include <config.h>
#include <nc_tests.h>
#include <nc4internal.h>
#include "err_macros.h"

#define FILE_NAME "tst_dims.nc"
#define LAT_NAME "lat"
#define LAT_LEN 1

// static int ncesdm_container_commit(nc_esdm_t *e) {
//   int ret;
//   // store the dimension table
//   int len = e->dimt.count;
//   smd_dtype_t *arr_type = smd_type_array(SMD_DTYPE_STRING, len);
//   smd_attr_t *new = smd_attr_new("_nc_dims", arr_type, e->dimt.name, 0);
//   esdm_container_link_attribute(e->c, 1, new);
//   //smd_type_unref(arr_type);
//
//   arr_type = smd_type_array(SMD_DTYPE_UINT64, len);
// =====>>> //   new = smd_attr_new("_nc_sizes", arr_type, e->dimt.size, 0);
// =====>>> //   esdm_container_link_attribute(e->c, 1, new);
//   //smd_type_unref(arr_type);
//
//   ret = esdm_container_commit(e->c);
//   if (ret != ESDM_SUCCESS) {
//     return NC_EBADID;
//   }
//   return NC_NOERR;
// }

int main(int argc, char **argv) {
  // #define VERY_LONG_LEN (size_t)4500000000LL
  
#define VERY_LONG_LEN (size_t)450
  int ncid, dimid;
  size_t len_in;
  char name_in[NC_MAX_NAME + 1];

  if (SIZEOF_SIZE_T == 8) {
    /* Create a file with one dim and nothing else. */
    if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
    if (nc_def_dim(ncid, LAT_NAME, VERY_LONG_LEN, &dimid)) ERR;

    /* Check it out. */
    if (nc_inq_dim(ncid, dimid, name_in, &len_in)) ERR;
    if (len_in != ((SIZEOF_SIZE_T == 8) ? VERY_LONG_LEN : NC_MAX_UINT) || strcmp(name_in, LAT_NAME)) ERR;
    if (nc_close(ncid)) ERR;

    /* Reopen and check it out again. */
    if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
    /* Check it out. */
    if (nc_inq_dim(ncid, dimid, name_in, &len_in)) ERR;
    if (len_in != ((SIZEOF_SIZE_T == 8) ? VERY_LONG_LEN : NC_MAX_UINT) || strcmp(name_in, LAT_NAME)) ERR;
    if (nc_close(ncid)) ERR;
  }
  SUMMARIZE_ERR;
  FINAL_RESULTS;
}
