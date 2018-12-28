include $(top_srcdir)/lib_flags.am
ESDMTESTS = tst_select
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

CLEANFILES = tst_*.esdm
