include $(top_srcdir)/lib_flags.am
ESDMTESTS = tst_select tst_attr tst_attr_global tst_attr_local tst_attr_local_short tst_attr_local_string tst_attr_new
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

CLEANFILES = tst_*.esdm
