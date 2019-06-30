/*********************************************************************
 *   Copyright 2016, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *********************************************************************/

#ifndef D4INCLUDES_H
#define D4INCLUDES_H 1

#include "config.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#include <curl/curl.h>

#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif

#ifdef HAVE_GETRLIMIT
#  ifdef HAVE_SYS_RESOURCE_H
#    include <sys/time.h>
#  endif
#  ifdef HAVE_SYS_RESOURCE_H
#    include <sys/resource.h>
#  endif
#endif

#include "nc.h"
#include "ncbytes.h"
#include "ncdap.h"
#include "nclist.h"
#include "nclog.h"
#include "ncrc.h"
#include "ncuri.h"
#include "netcdf.h"

#include "d4util.h"

#include "ncd4.h"
#include "ncd4types.h"

#include "d4chunk.h"
#include "d4debug.h"

#endif /*D4INCLUDES_H*/
