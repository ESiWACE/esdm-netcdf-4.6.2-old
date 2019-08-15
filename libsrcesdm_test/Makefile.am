include $(top_srcdir)/lib_flags.am
ESDMTESTS = tst_select tst_attr tst_attr_global tst_attr_local_string tst_dims tst_new tst_var tst_dim tst_dim_ME tst_var_ME tst_att tst_rename2_ME tst_rename tst_rename2 tst_rename_ME tst_dims_ME tst_unlim_vars_ME tst_vars3_ME tst_attr_global_ME
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

CLEANFILES = tst_*.esdm
