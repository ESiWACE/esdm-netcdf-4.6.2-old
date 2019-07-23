include $(top_srcdir)/lib_flags.am
ESDMTESTS = tst_select tst_attr tst_attr_global tst_attr_local_string tst_dims tst_new tst_var tst_dim tst_dim_ME tst_var_ME tst_att
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

CLEANFILES = tst_*.esdm
