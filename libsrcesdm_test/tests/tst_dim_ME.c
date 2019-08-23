#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>

#include "netcdf.h"

#define FILE_NAME "tst_dim_ME.nc4"
#define TEST_FLAGS NC_ESDM

#define ERRCODE 2
#define ERR(e)                                          \
  {                                                     \
    printf("Error: %d %s\n", __LINE__, nc_strerror(e)); \
    exit(ERRCODE);                                      \
  }

static void write_test();
static void read_test();

// /*
// * Creates the dimension, but it does not reconstruct the dimension table
// *
//
// tst_dim_ME: ../../libsrcesdm_test/tst_dim_ME.c:70: read_test: Assertion `idp == 0' failed.
//
// [ESDM NC] called ESDM_inq_dimid:483 65536
// 485	  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
// (gdb)
// 486	  if(e == NULL) return NC_EBADID;
// (gdb) p *e
// $1 = {ncid = 65536, idp = 0, c = 0x55555579f1f0, dimt = {count = 0, size = 0x0, name = 0x0}, vars = {count = 0, var = 0x0}}
// *
// */

int main(int argc, char **argv) {
  nc_initialize();

  write_test();
  read_test();

  nc_finalize();

  printf("*** SUCCESS attributes!\n");
  return 0;
}

static void write_test() {
  int ncid, x_dimid, retval;

  if ((retval = nc_create(FILE_NAME, NC_NOWRITE | TEST_FLAGS, &ncid)))
    ERR(retval);

  if ((retval = nc_def_dim(ncid, "x", 106, &x_dimid)))
    ERR(retval);

  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  if (retval = nc_close(ncid))
    ERR(retval);
}

static void read_test() {
  int ncid, retval;

  if ((retval = nc_open(FILE_NAME, NC_CLOBBER | TEST_FLAGS, &ncid)))
    ERR(retval);

  int idp = 2;
  if ((retval = nc_inq_dimid(ncid, "x", &idp))) ERR(retval);
  assert(idp == 0);

  if ((retval = nc_enddef(ncid))) ERR(retval);
  if ((retval = nc_close(ncid))) ERR(retval);
}
