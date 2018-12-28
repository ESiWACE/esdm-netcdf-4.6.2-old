# This automake file generates the Makefile to build netCDF with ESDM support.

include $(top_srcdir)/lib_flags.am
AM_CPPFLAGS += -I$(top_srcdir)/libsrcesdm

SRC = esdm_dispatch.c

noinst_LTLIBRARIES = libnetcdfesdm.la

libnetcdfesdm_la_CPPFLAGS = ${AM_CPPFLAGS}
libnetcdfesdm_la_SOURCES = $(SRC)
libnetcdfesdm_la_LIBADD =
