#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include <math.h>

#define FILE_NAME "tst_bug1.nc"
#define VAR_NAME "Monkey"

// Create a file with a scalar variable (no dimensions)
// Insert a value in the variable
// Close and open the file

//Error: tst_bug1: /home/lucy/esiwace/esdm/src/esdm-datatypes.c:674:
//esdmI_create_fragment_from_metadata: Assertion `dims > 0' failed.

int main(int argc, char **argv) {
  int ncid, varid;
  signed char schar = 2, schar2;

  /* Write a scalar NC_BYTE with value 2. */
  if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
  if (nc_def_var(ncid, VAR_NAME, NC_BYTE, 0, NULL, &varid)) ERR;
  if (nc_put_var_schar(ncid, varid, &schar)) ERR;
  if (nc_close(ncid)) ERR;

  /* Now open the file and check it. */
  if (nc_open(FILE_NAME, NC_NOWRITE, &ncid)) ERR;
  if (nc_get_var_schar(ncid, varid, &schar2)) ERR;
  if (nc_close(ncid)) ERR;
  printf("%d %d\n", schar, schar2);
  assert(schar == schar2);

  FINAL_RESULTS;
}

// Backtrace
// #0  __GI_raise (sig=sig@entry=6) at ../sysdeps/unix/sysv/linux/raise.c:51
// #1  0x00007ffff7705801 in __GI_abort () at abort.c:79
// #2  0x00007ffff76f539a in __assert_fail_base (fmt=0x7ffff787c7d8 "%s%s%s:%u: %s%sAssertion `%s' failed.\n%n",
//     assertion=assertion@entry=0x7ffff552ed54 "dims > 0",
//     file=file@entry=0x7ffff552d0c8 "/home/lucy/esiwace/esdm/src/esdm-datatypes.c", line=line@entry=674,
//     function=function@entry=0x7ffff552e040 "esdmI_create_fragment_from_metadata") at assert.c:92
// #3  0x00007ffff76f5412 in __GI___assert_fail (assertion=0x7ffff552ed54 "dims > 0",
//     file=0x7ffff552d0c8 "/home/lucy/esiwace/esdm/src/esdm-datatypes.c", line=674,
//     function=0x7ffff552e040 "esdmI_create_fragment_from_metadata") at assert.c:101
// #4  0x00007ffff55252b0 in esdmI_create_fragment_from_metadata (dset=0x555555821640, json=0x555555821830,
//     out=0x7fffffffdc08) at /home/lucy/esiwace/esdm/src/esdm-datatypes.c:674
// #5  0x00007ffff5526150 in esdm_dataset_open_md_parse (d=0x555555821640,
//     md=0x5555557a1070 "{\"Variables\":{\"type\":\"c\",\"data\":null}{\"id\":\"fe2DKbbzj9Y0kEpk\",\"typ\":\"e\",\"dims\":0,\"dims_dset_id\":[],\"fragments\":[{\"id\":\"fe2DKbxOPzPXyKqLrJHZ\",\"pid\":\"p1\",\"size\":[1],\"offset\":[0]}]}", size=178) at /home/lucy/esiwace/esdm/src/esdm-datatypes.c:868
// #6  0x00007ffff55262b5 in esdm_dataset_ref (d=0x555555821640) at /home/lucy/esiwace/esdm/src/esdm-datatypes.c:896
// #7  0x00007ffff7b4728e in ESDM_open (path=0x55555579fa10 "tst_bug1.nc", cmode=0, basepe=0, chunksizehintp=0x0,
//     parameters=0x0, table=0x7ffff7dca1c0 <esdm_dispatcher>, ncp=0x5555557a1130)
//     at ../../libsrcesdm/esdm_dispatch.c:324
// #8  0x00007ffff7ada19b in NC_open (path0=0x555555554f4b "tst_bug1.nc", omode=0, basepe=0, chunksizehintp=0x0,
//     useparallel=0, parameters=0x0, ncidp=0x7fffffffdef0) at ../../libdispatch/dfile.c:2379
// #9  0x00007ffff7ad8d51 in nc_open (path=0x555555554f4b "tst_bug1.nc", omode=0, ncidp=0x7fffffffdef0)
//     at ../../libdispatch/dfile.c:801
// #10 0x0000555555554d68 in main (argc=1, argv=0x7fffffffdfe8) at ../../libsrcesdm_test/tst_bug1.c:25
