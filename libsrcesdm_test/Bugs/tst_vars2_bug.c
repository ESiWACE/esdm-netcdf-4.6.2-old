#include <nc_tests.h>
#include "err_macros.h"
#include "netcdf.h"
#include "netcdf_f.h"

#define FILE_NAME "tst_vars2.nc"
#define NUM_DIMS 1
#define NUM_VARS 3
#define DIM1_LEN NC_UNLIMITED
#define DIM1_NAME "Hoplites_Engaged"
#define VAR_NAME "Battle_of_Marathon"
#define LOSSES_NAME "Miltiades_Losses"
#define NDIMS1 1
#define MAX_CNUM 4

// if (nc_free_string(NLINES, (char **)speech_in)) ERR; (95)

int main(int argc, char **argv) {
  int ncid, dimids[NUM_DIMS];
  int varid;
  int nvars_in, varids_in[NUM_VARS] = {0};
  signed char fill_value = 42, fill_value_in;
  nc_type xtype_in;
  size_t len_in;
  char name_in[NC_MAX_NAME + 1];
  int attnum_in;
  int cnum;
  char too_long_name[NC_MAX_NAME + 2];

  /* Set up a name that is too long for netCDF. */
  memset(too_long_name, 'a', NC_MAX_NAME + 1);
  too_long_name[NC_MAX_NAME + 1] = 0;


#define NDIMS 3
#define NNAMES 4
#define NLINES 13
  printf("**** testing funny names for netCDF-4...");
  {
    int ncid, wind_id;
    size_t len[NDIMS] = {7, 3, 1};
    int dimids[NDIMS], dimids_in[NDIMS], ndims_in;
    char funny_name[NNAMES][NC_MAX_NAME] = {"\a\t", "\f\n", "\r\v", "\b"};
    char serious_name[NNAMES][NC_MAX_NAME] = {"name1", "name2", "name3", "name4"};
    char name_in[NC_MAX_NAME + 1];
    char *speech[NLINES] = {"who would fardels bear, ",
    "To grunt and sweat under a weary life, ",
    "But that the dread of something after death, ",
    "The undiscover'd country from whose bourn ",
    "No traveller returns, puzzles the will ",
    "And makes us rather bear those ills we have ",
    "Than fly to others that we know not of? ",
    "Thus conscience does make cowards of us all; ",
    "And thus the native hue of resolution ",
    "Is sicklied o'er with the pale cast of thought, ",
    "And enterprises of great pith and moment ",
    "With this regard their currents turn awry, ",
    "And lose the name of action."};
    char *speech_in[NLINES];
    int i;
    unsigned short nlines = NLINES;
    unsigned int nlines_in;

    if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

    // Define dimensions.
    for (i = 0; i < NDIMS; i++)
      if (nc_def_dim(ncid, serious_name[i], len[i], &dimids[i])) ERR;

    // Write some global atts.
    if (nc_put_att_string(ncid, NC_GLOBAL, serious_name[0], NLINES,
        (const char **)speech)) ERR;
    if (nc_put_att_ushort(ncid, NC_GLOBAL, serious_name[1], NC_UINT, 1, &nlines)) ERR;
    //
    // // Define variables.
    if (nc_def_var(ncid, serious_name[3], NC_INT64, NDIMS, dimids, &wind_id)) ERR;

    if (nc_close(ncid)) ERR;

    // Open the file and check.
    if (nc_open(FILE_NAME, NC_WRITE, &ncid)) ERR;
    if (nc_inq_dimids(ncid, &ndims_in, dimids_in, 0)) ERR;
    if (ndims_in != NDIMS) ERR;
    for (i = 0; i < NDIMS; i++) {
      if (dimids_in[i] != i) ERR;
      if (nc_inq_dimname(ncid, i, name_in)) ERR;
      if (strcmp(name_in, serious_name[i])) ERR;
    }

    if (nc_get_att_string(ncid, NC_GLOBAL, serious_name[0], (char **)speech_in)) ERR;
    for (i = 0; i < NLINES; i++)
      if (strcmp(speech_in[i], speech[i])) ERR;
    if (nc_get_att_uint(ncid, NC_GLOBAL, serious_name[1], &nlines_in)) ERR;
    if (nlines_in != NLINES) ERR;
    // if (nc_free_string(NLINES, (char **)speech_in)) ERR;
    if (nc_inq_varname(ncid, 0, name_in)) ERR;
    if (strcmp(name_in, serious_name[3])) ERR;
    if (nc_close(ncid)) ERR;
  }
  SUMMARIZE_ERR;
  FINAL_RESULTS;
}
