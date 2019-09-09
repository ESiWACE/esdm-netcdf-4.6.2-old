include $(top_srcdir)/lib_flags.am
ESDMTESTS = h5testszip tst_select tst_attr tst_attr_global tst_attr_local_string tst_dims tst_new tst_var tst_dim tst_dim_ME tst_var_ME tst_att tst_rename2_ME tst_rename tst_rename2 tst_rename_ME tst_dims_ME tst_unlim_vars_ME tst_vars3_ME tst_attr_global_ME tst_dim0_ME_1 tst_dim0_ME_2 tst_dim0_ME_3 tst_smd tst_dims3 cdm_sea_soundings openbigmeta ref_bzip2 renamegroup test_filter test_filter_misc test_szip tst_ar4_3d tst_ar4_4d tst_ar4 tst_atts1 tst_atts2 tst_atts3 tst_atts tst_atts_mod tst_attsperf tst_atts-simple tst_atts_string_rewrite tst_bug324 tst_camrun tst_chunks2 tst_chunks3 tst_chunks tst_compounds2 tst_compounds3 tst_compounds tst_converts2 tst_converts tst_coords2 tst_coords3 tst_coords tst_create_files tst_dims2 tst_dims3 tst_dims tst_elatefill tst_empty_vlen_unlim tst_endian_fill tst_enums tst_files2 tst_files3 tst_files4 tst_files5 tst_files6 tst_files tst_fill_attr_vanish tst_fillbug tst_fills2 tst_fills tst_filterparser tst_grps2 tst_grps tst_h5_endians tst_hdf5_file_compat tst_h_many_atts tst_h_refs tst_h_scalar tst_h_strbug tst_interops4 tst_interops5 tst_interops6 tst_interops tst_knmi tst_large2 tst_large tst_mem tst_mode tst_mpi_parallel tst_nc4perf tst_opaques tst_parallel3 tst_parallel4 tst_parallel5 tst_parallel tst_put_vars tst_put_vars_two_unlim_dim tst_rehash tst_rename2 tst_rename tst_simplerw_coll_r tst_strings tst_strings2 tst_sync tst_types tst_udf tst_unlim_vars tst_utf8 tst_utils tst_v2 tst_varms tst_vars2 tst_vars3 tst_vars4 tst_vars tst_vl tst_xplatform2 tst_xplatform t_type tst_bug1 tst_bug2 tst_bug3 tst_bug4 tst_bug5 tst_bug6 tst_bug7 tst_bug8 tst_bug9 tst_bug10 tst_bug11 tst_bug12 tst_bug13 tst_bug14 tst_bug15 tst_bug16 tst_bug17 tst_bug18 tst_bug19 tst_bug20 tst_bug21 tst_bug22 tst_bug23 tst_bug24 tst_bug25 tst_bug26 tst_bug27 tst_bug28 tst_bug29 tst_bug30 tst_bug31 tst_bug32 tst_bug33 tst_bug34 tst_bug35 tst_bug36 tst_bug37 tst_bug38 tst_bug39 tst_bug40 tst_bug41
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

#SUBDIRS = esdm_nc_test4

CLEANFILES = tst_*.esdm
