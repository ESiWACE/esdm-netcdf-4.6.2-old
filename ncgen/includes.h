/*
  This is part of ncgen, a utility which is part of netCDF. Copyright
  2010, UCAR/Unidata. See COPYRIGHT file for copying and
  redistribution conditions.
 */

/* $Id: includes.h,v 1.7 2010/05/25 18:57:00 dmh Exp $ */
/* $Header: /upc/share/CVS/netcdf-3/ncgen/includes.h,v 1.7 2010/05/25 18:57:00 dmh Exp $ */

#ifndef NCGEN_INCLUDES_H
#define NCGEN_INCLUDES_H

#include "config.h"

#include <assert.h>
#include <ctype.h> /* for isprint() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef __SunOS
#  include <strings.h>
#endif

#ifdef __hpux
#  include <locale.h>
#endif

#include "bytebuffer.h"
#include "list.h"
#include "nctime.h"

/* Local Configuration flags*/
#define ENABLE_BINARY
#define ENABLE_C
#define ENABLE_F77
#define ENABLE_JAVA

#include "data.h"
#include "debug.h"
#include "genlib.h"
#include "nc.h"
#include "ncgen.h"
#include "netcdf.h"
#include "util.h"
#ifdef USE_NETCDF4
#  include "nc4internal.h"
#endif

extern int specialconstants;

#undef ITERBUG
#undef CHARBUG

#undef nullfree
#define nullfree(x)   \
  {                   \
    if ((x)) free(x); \
  }

#endif /* NCGEN_INCLUDES_H */
