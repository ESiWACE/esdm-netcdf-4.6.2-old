#include <config.h>
#include <nc_tests.h>
#include "err_macros.h"
#include "hdf5internal.h"

#define FILE_NAME "tst_atts.nc"
#define OLD_NAME "Constantinople"
#define OLD_NAME_2 "Constantinopolis"
#define CONTENTS "Lots of people!"

// if (nc_enddef(ncid)) ERR;

int main(int argc, char **argv) {

  SUMMARIZE_ERR;
  printf("*** testing deleting atts...");
  {
    int ncid;
    int natts;

    /* Create a file with two atts. */
    if (nc_create(FILE_NAME, NC_NETCDF4 | NC_CLOBBER, &ncid)) ERR;
    if (nc_put_att_text(ncid, NC_GLOBAL, OLD_NAME, strlen(CONTENTS),
        CONTENTS)) ERR;
    if (nc_put_att_text(ncid, NC_GLOBAL, OLD_NAME_2, 0, NULL)) ERR;

    /* End define mode. It redef will be called automatically. */
    if (nc_enddef(ncid)) ERR;

    /* Delete the attribute. */
    if (nc_del_att(ncid, NC_GLOBAL, OLD_NAME)) ERR;
    if (nc_close(ncid)) ERR;

    /* Reopen the file. */
    if (nc_open(FILE_NAME, 0, &ncid)) ERR;
    if (nc_inq_natts(ncid, &natts)) ERR;
    printf("\n\nnatts=%d\n\n", natts);
    if (natts != 1) ERR;
    if (nc_close(ncid)) ERR;
  }

  FINAL_RESULTS;
}
