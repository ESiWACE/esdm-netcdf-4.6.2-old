include $(top_srcdir)/lib_flags.am
ESDMTESTS = cdm_sea_soundings h5testszip openbigmeta ref_bzip2 renamegroup t_type test_filter test_filter_misc test_szip tst_ar4 tst_ar4_3d tst_ar4_4d tst_atts tst_atts-simple tst_atts1 tst_atts2 tst_atts3 tst_atts_mod tst_atts_string_rewrite tst_attsperf tst_bug324 tst_bug45 tst_bug46 tst_bug47 tst_camrun tst_chunks tst_chunks2 tst_chunks3 tst_compounds tst_compounds2 tst_compounds3 tst_converts tst_converts2 tst_coords tst_coords2 tst_coords3 tst_create_files tst_dims tst_dims2 tst_dims3 tst_elatefill tst_empty_vlen_unlim tst_endian_fill tst_enums tst_files tst_files2 tst_files3 tst_files4 tst_files5 tst_files6 tst_fill_attr_vanish tst_fillbug tst_fills tst_fills2 tst_filterparser tst_grps tst_grps2 tst_h5_endians tst_h_many_atts tst_h_refs tst_h_scalar tst_h_strbug tst_hdf5_file_compat tst_interops tst_interops4 tst_interops5 tst_interops6 tst_knmi tst_large tst_large2 tst_mem tst_mode tst_mpi_parallel tst_nc4perf tst_opaques tst_parallel tst_parallel3 tst_parallel4 tst_parallel5 tst_put_vars tst_put_vars_two_unlim_dim tst_rehash tst_rename tst_rename2 tst_simplerw_coll_r tst_strings tst_strings2 tst_sync tst_types tst_udf tst_unlim_vars tst_utf8 tst_v2 tst_varms tst_vars tst_vars2 tst_vars3 tst_vars4 tst_vl tst_xplatform
check_PROGRAMS = $(ESDMTESTS)
TESTS = $(ESDMTESTS)

tst_ar4_SOURCES = tst_ar4.c tst_utils.c
bm_file_SOURCES = bm_file.c tst_utils.c
bm_many_atts_SOURCES = bm_many_atts.c tst_utils.c
bm_many_objs_SOURCES = bm_many_objs.c tst_utils.c
tst_ar4_SOURCES = tst_ar4.c tst_utils.c
tst_ar4_3d_SOURCES = tst_ar4_3d.c tst_utils.c
tst_ar4_4d_SOURCES = tst_ar4_4d.c tst_utils.c
tst_files2_SOURCES = tst_files2.c tst_utils.c
tst_files3_SOURCES = tst_files3.c tst_utils.c
tst_h_many_atts_SOURCES = tst_h_many_atts.c tst_utils.c
tst_knmi_SOURCES = tst_knmi.c tst_utils.c

AM_LDFLAGS += ${top_builddir}/liblib/libnetcdf.la
LDADD = ${top_builddir}/liblib/libnetcdf.la

#SUBDIRS = esdm_nc_test4

CLEANFILES = tst_*.esdm
