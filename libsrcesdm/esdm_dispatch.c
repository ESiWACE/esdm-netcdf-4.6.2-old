#include <stdlib.h>
#include <assert.h>

#include <netcdf_meta.h>
#include <mpi.h>

#include <esdm.h>

#ifdef NC_HAS_PARALLEL
#  define ESDM_PARALLEL
#  warning "USING ESDM PARALLEL"
#else
#  warning "USING ESDM IS NOT PARALLEL"
#endif

#ifdef ESDM_PARALLEL
#  include <esdm-mpi.h>
#endif

#include "config.h"
#include "nc.h"
#include "ncdispatch.h"

#define NOT_IMPLEMENTED assert(0 && "NOT IMPLEMENTED");

#define DEBUG(...)                                        \
  do {                                                    \
    printf("[ESDM NC] %s(Line:%d) ", __func__, __LINE__); \
    printf(__VA_ARGS__);                                  \
  } while (0)

#define ERROR(...)                                        \
  do {                                                    \
    printf("[ESDM NC] ERROR %s(Line:%d) ", __func__, __LINE__); \
    printf(__VA_ARGS__);                                  \
    exit(1);                                              \
  } while (0)

#ifdef DEBUG_MORE
#  define DEBUG_ENTER(...)                                   \
    do {                                                     \
      printf("[ESDM NC] called %s(Line:%d) ", __func__, __LINE__); \
      printf(__VA_ARGS__);                                   \
    } while (0)
#else
#  define DEBUG_ENTER(...)
#endif

#define WARN(...)                                        \
  do {                                                   \
    printf("[ESDM NC] WARN %s(Line:%d) ", __func__, __LINE__); \
    printf(__VA_ARGS__);                                 \
  } while (0)

#define WARN_NOT_IMPLEMENTED                                                \
  do {                                                                      \
    printf("[ESDM NC] WARN %s():%d NOT IMPLEMENTED\n", __func__, __LINE__); \
  } while (0)

#define WARN_NOT_SUPPORTED                                               \
  do {                                                                   \
    printf(                                                              \
    "[ESDM NC] WARN %s():%d. NetCDF Feature not supported with ESDM!\n", \
    __func__, __LINE__);                                                 \
  } while (0)

#define WARN_NOT_SUPPORTED_TYPES                                         \
  do {                                                                   \
    printf("[ESDM NC] WARN %s():%d. ESDM does not support user-defined " \
           "datatypes from NetCDF!\n",                                   \
    __func__, __LINE__);                                                 \
  } while (0)

#define WARN_NOT_SUPPORTED_ENDIAN                                             \
  do {                                                                        \
    printf("[ESDM NC] WARN %s():%d. ESDM only supports native-endianness!\n", \
    __func__, __LINE__);                                                      \
  } while (0)

#define WARN_NOT_SUPPORTED_FILTER                                      \
  do {                                                                 \
    printf("[ESDM NC] WARN %s():%d. ESDM does not support filters!\n", \
    __func__, __LINE__);                                               \
  } while (0)

#define WARN_NOT_SUPPORTED_GROUPS                                          \
  do {                                                                     \
    printf(                                                                \
    "[ESDM NC] WARN %s():%d. ESDM does not support groups from NetCDF!\n", \
    __func__, __LINE__);                                                   \
  } while (0)

#define WARN_NOT_SUPPORTED_COMPRESSION                                     \
  do {                                                                     \
    printf("[ESDM NC] WARN %s():%d. ESDM does not support compression!\n", \
    __func__, __LINE__);                                                   \
  } while (0)

typedef struct {
  int *dimidsp;
  int fillmode; // remembers if fill mode is on or off, by default fill mode is
                // on, actually, ESDM with NetCDF always has a fill mode, uses
                // the defaults from NetCDF
  esdm_dataset_t *dset;
} md_var_t;

typedef struct {
  int count;
  md_var_t **var;
} md_vars_t;

typedef struct {
  int count;
  uint64_t *size;
  char **name;
} nc_dim_tbl_t;

typedef struct {
  int ncid;
  int fillmode; // remembers if fill mode is on or off, by default fill mode is
                // on, actually, ESDM with NetCDF always has a fill mode, uses
                // the defaults from NetCDF
  esdm_container_t *c;
  int original_nc;
  // Some attributes provide information about the dataset as a whole and are
  // called global attributes. These are identified by the attribute name
  // together with a blank variable name (in CDL) or a special null "global
  // variable" ID (in C or Fortran).
  nc_dim_tbl_t dimt;
  md_vars_t vars;
  int parallel_mode;
#ifdef ESDM_PARALLEL
  MPI_Comm comm;
#endif
} nc_esdm_t;

static esdm_type_t type_nc_to_esdm(nc_type type) {
  switch (type) {
    case (NC_NAT):
      return SMD_DTYPE_UNKNOWN;
    case (NC_BYTE):
      return SMD_DTYPE_INT8;
    case (NC_CHAR):
      return SMD_DTYPE_CHAR;
    case (NC_SHORT):
      return SMD_DTYPE_INT16;
    case (NC_INT):
      return SMD_DTYPE_INT32;
    case (NC_FLOAT):
      return SMD_DTYPE_FLOAT;
    case (NC_DOUBLE):
      return SMD_DTYPE_DOUBLE;
    case (NC_UBYTE):
      return SMD_DTYPE_UINT8;
    case (NC_USHORT):
      return SMD_DTYPE_UINT16;
    case (NC_UINT):
      return SMD_DTYPE_UINT32;
    case (NC_INT64):
      return SMD_DTYPE_INT64;
    case (NC_UINT64):
      return SMD_DTYPE_UINT64;
    case (NC_STRING):
      return SMD_DTYPE_STRING;
    default:
      printf("ESDM does not support compound datatypes from NetCDF: %d\n", type);
      return NULL;
  }
}

static nc_type type_esdm_to_nc(smd_basic_type_t type) {
  switch (type) {
    case (SMD_TYPE_UNKNOWN):
      return NC_NAT;
    case (SMD_TYPE_INT8):
      return NC_BYTE;
    case (SMD_TYPE_CHAR):
      return NC_CHAR;
    case (SMD_TYPE_INT16):
      return NC_SHORT;
    case (SMD_TYPE_INT32):
      return NC_INT;
    case (SMD_TYPE_FLOAT):
      return NC_FLOAT;
    case (SMD_TYPE_DOUBLE):
      return NC_DOUBLE;
    case (SMD_TYPE_UINT8):
      return NC_UBYTE;
    case (SMD_TYPE_UINT16):
      return NC_USHORT;
    case (SMD_TYPE_UINT32):
      return NC_UINT;
    case (SMD_TYPE_INT64):
      return NC_INT64;
    case (SMD_TYPE_UINT64):
      return NC_UINT64;
    case (SMD_TYPE_STRING):
      return NC_STRING;
    default:
      printf("Unsupported datatype: %d\n", type);
      return 0;
  }
}

static inline nc_esdm_t *ESDM_nc_get_esdm_struct(int ncid) {
  NC *ncp;
  if (NC_check_id(ncid, (NC **)&ncp) != NC_NOERR)
    return NULL;
  nc_esdm_t *e = (nc_esdm_t *)ncp->dispatchdata;
  if (e->ncid != ncid)
    return NULL;
  return e;
}

static void set_default_fill_mode(esdm_dataset_t *dset) {
  esdm_dataspace_t *space;
  esdm_status status = esdm_dataset_get_dataspace(dset, &space);
  esdm_type_t type = esdm_dataspace_get_type(space);
  int value[2];
  switch (type->type) {
    case (SMD_TYPE_INT8):
      *(int8_t *)value = NC_FILL_BYTE;
      break;
    case (SMD_TYPE_INT16):
      *(int16_t *)value = NC_FILL_SHORT;
      break;
    case (SMD_TYPE_INT32):
      *(int32_t *)value = NC_FILL_INT;
      break;
    case (SMD_TYPE_INT64):
      *(int64_t *)value = NC_FILL_INT64;
      break;
    case (SMD_TYPE_UINT8):
      *(uint8_t *)value = NC_FILL_UBYTE;
      break;
    case (SMD_TYPE_UINT16):
      *(uint16_t *)value = NC_FILL_USHORT;
      break;
    case (SMD_TYPE_UINT32):
      *(uint32_t *)value = NC_FILL_UINT;
      break;
    case (SMD_TYPE_UINT64):
      *(uint64_t *)value = NC_FILL_UINT64;
      break;
    case (SMD_TYPE_FLOAT):
      *(float *)value = NC_FILL_FLOAT;
      break;
    case (SMD_TYPE_DOUBLE):
      *(double *)value = NC_FILL_DOUBLE;
      break;
    case (SMD_TYPE_CHAR):
      *(char *)value = NC_FILL_CHAR;
      break;
    case (SMD_TYPE_STRING):
      *(char **)value = NC_FILL_STRING;
      break;
    default:
      assert(0 && "Not supported");
      return;
  }
  status = esdm_dataset_set_fill_value(dset, &value);
  assert(status == ESDM_SUCCESS);

  smd_attr_t *attrs;
  status = esdm_dataset_get_attributes(dset, &attrs);
  int dims_pos = smd_find_position_by_name(attrs, "_FillValue");
  if (dims_pos >= 0) {
    smd_attr_unlink_pos(attrs, dims_pos);
  }
  //smd_attr_t * attr = smd_attr_new("_FillValue", type, & value);
  //status = esdm_dataset_link_attribute(dset, 1, attr);
}

static void add_to_dims_tbl(nc_esdm_t *e, char const *name, size_t size) {
  int cur = e->dimt.count;
  e->dimt.count++;
  int new = e->dimt.count;
  e->dimt.name = realloc(e->dimt.name, new * sizeof(void *));
  e->dimt.size = realloc(e->dimt.size, new * sizeof(size_t));

  if (!e->dimt.name || !e->dimt.size) {
    ERROR("Cannot allocate memory.");
  }

  e->dimt.name[cur] = strdup(name);
  e->dimt.size[cur] = size;
}

static void ncesdm_remove_attr(esdm_container_t *c) {
  smd_attr_t *attrs;
  int status = esdm_container_get_attributes(c, &attrs);
  int dims_pos = smd_find_position_by_name(attrs, "_nc_dims");
  if (dims_pos >= 0)
    smd_attr_unlink_pos(attrs, dims_pos);
  int sizes_pos = smd_find_position_by_name(attrs, "_nc_sizes");
  if (sizes_pos >= 0)
    smd_attr_unlink_pos(attrs, sizes_pos);
}

static size_t esdm_container_dataset_get_actual_size(nc_esdm_t *e, int dimid) {
  // it should be easier to get the actual size

  int nvars = e->vars.count;
  md_var_t *ev;
  size_t max = 0;

  // find a dataset that contains the dimension
  for (int varid = 0; varid < nvars; varid++) {
    ev = e->vars.var[varid];

    esdm_dataspace_t *space;
    esdm_status status = esdm_dataset_get_dataspace(ev->dset, &space);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;

    int64_t ndims = esdm_dataspace_get_dims(space);
    int64_t const *sizes = esdm_dataset_get_actual_size(ev->dset);
    for (int i = 0; i < ndims; i++) {
      if (dimid == ev->dimidsp[i]) {
        size_t cur = sizes[i];
        max = max < cur ? cur : max;
      }
    }
  }

  if (max == 0) {
    // there is no dataset containing the dimension
    return (e->dimt.size[dimid]);
  }
  return max;
}

static int ncesdm_container_commit(nc_esdm_t *e) {
  esdm_status status;
  if(e->original_nc){
    // store the dimension table
    int len = e->dimt.count;
    smd_dtype_t *arr_type = smd_type_array(SMD_DTYPE_STRING, len);
    smd_attr_t *new = smd_attr_new("_nc_dims", arr_type, e->dimt.name);
    esdm_container_link_attribute(e->c, 1, new);
    // smd_type_unref(arr_type);

    arr_type = smd_type_array(SMD_DTYPE_UINT64, len);
    new = smd_attr_new("_nc_sizes", arr_type, e->dimt.size);
    esdm_container_link_attribute(e->c, 1, new);
    // smd_type_unref(arr_type);
  }

#ifdef ESDM_PARALLEL
  if (e->parallel_mode) {
    status = esdm_mpi_container_commit(e->comm, e->c);
  } else {
    status = esdm_container_commit(e->c);
  }
#else
  status = esdm_container_commit(e->c);
#endif

  if (status != ESDM_SUCCESS) {
    return NC_EBADID;
  }
  return NC_NOERR;
}

int lookup_md(md_vars_t *md, char *name, md_var_t **value, int *pos) {
  for (int i = 0; i < md->count; i++) {
    if (strcmp(name, esdm_dataset_name(md->var[i]->dset)) == 0) {
      *value = md->var[i];
      *pos = i;
      return NC_NOERR;
    }
  }
  return NC_EBADID;
}

void insert_md(md_vars_t *md, md_var_t *value) {
  md->count++;
  md->var = realloc(md->var, md->count * sizeof(void *));
  if (!md->var) {
    ERROR("Cannot allocate memory.");
  }
  md->var[md->count - 1] = value;
}

/**
 * @brief
 * @param
 */

int ESDM_create(const char *path, int cmode, size_t initialsz, int basepe, size_t *chunksizehintp, void *parameters, struct NC_Dispatch *table, NC *ncp) {
  DEBUG_ENTER("%s\n", path);
  const char *realpath = path;

  if (strncmp(path, "esdm:", 5) == 0) {
    realpath = &path[5];
  }
  // const char * base = basename(realpath);
  // remove leading slashes
  while (realpath[0] == '/') {
    realpath++;
  }
  char *cpath = strdup(realpath);
  // remove trailing slashes
  int pos = strlen(cpath) - 1;
  for (; pos > 0; pos--) {
    if (cpath[pos] != '/') {
      break;
    }
    cpath[pos] = '\0';
  }
  DEBUG_ENTER("%s %d %d %s\n", cpath, ncp->ext_ncid, ncp->int_ncid, ncp->path);

  nc_esdm_t *e = malloc(sizeof(nc_esdm_t));
  memset(e, 0, sizeof(nc_esdm_t));
  e->ncid = ncp->ext_ncid;
  e->original_nc = 1; // this will be an originally created netcdf file

  esdm_status status;

#ifdef ESDM_PARALLEL
  NC_MPI_INFO *data = (NC_MPI_INFO *)(parameters);
  if (data) {
    MPI_Comm_dup(data->comm, &e->comm);
    e->parallel_mode = 1;
    status = esdm_mpi_container_create(e->comm, cpath, cmode & NC_NOCLOBBER ? 0 : 1, &e->c);
  } else {
    e->parallel_mode = 0;
    status = esdm_container_create(cpath, cmode & NC_NOCLOBBER ? 0 : 1, &e->c);
  }
#else
  status = esdm_container_create(cpath, cmode & NC_NOCLOBBER ? 0 : 1, &e->c);
#endif

  if (status != ESDM_SUCCESS) {
    return NC_EEXIST;
  }

  free(cpath);
  ncp->dispatchdata = e;

  return NC_NOERR;
}

/**
 * @brief Open an existing netCDF file.
 * @param path	Must be non-null, but otherwise only used to set the dataset name.
 * @param omode	the open mode flags; Note that this procedure uses a limited set of flags because it forcibly sets NC_INMEMORY.
 * @param params	controlling parameters
 * @param ncidp	Pointer to location where returned netCDF ID is to be stored.
 */

int ESDM_open(const char *path, int cmode, int basepe, size_t *chunksizehintp, void *parameters, struct NC_Dispatch *table, NC *ncp) {
  const char *realpath = path;
  DEBUG_ENTER("%s\n", path);

  if (strncmp(path, "esdm:", 5) == 0) {
    realpath = &path[5];
  } else if (strncmp(path, "esd:", 4) == 0) {
    realpath = &path[4];
  }
  // remove leading slashes
  while (realpath[0] == '/') {
    realpath++;
  }
  char *cpath = strdup(realpath);
  // remove trailing slashes
  int pos = strlen(cpath) - 1;
  for (; pos > 0; pos--) {
    if (cpath[pos] != '/') {
      break;
    }
    cpath[pos] = '\0';
  }
  // const char * base = basename(realpath);

  DEBUG_ENTER("%s %d %d %s\n", cpath, ncp->ext_ncid, ncp->int_ncid, ncp->path);

  nc_esdm_t *e = malloc(sizeof(nc_esdm_t));
  memset(e, 0, sizeof(nc_esdm_t));
  e->ncid = ncp->ext_ncid;

  esdm_status status;

#ifdef ESDM_PARALLEL
  NC_MPI_INFO *data = (NC_MPI_INFO *)(parameters);
  if (data) {
    MPI_Comm_dup(data->comm, &e->comm);
    e->parallel_mode = 1;
    status = esdm_mpi_container_open(e->comm, cpath, 0, &e->c);
  } else {
    e->parallel_mode = 0;
    status = esdm_container_open(cpath, 0, &e->c);
  }
#else
  status = esdm_container_open(cpath, 0, &e->c);
#endif

  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  free(cpath);

  ncp->dispatchdata = e;

  // now build the dim table
  esdm_container_t *c = e->c;
  int ndsets = esdm_container_dataset_count(c);
  // open all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++) {
    esdm_dataset_t *dset = esdm_container_dataset_from_array(c, i);

#ifdef ESDM_PARALLEL
    if (e->parallel_mode) {
      status = esdm_mpi_dataset_ref(e->comm, dset);
    } else {
      status = esdm_dataset_ref(dset);
    }
#else
    status = esdm_dataset_ref(dset);
#endif
    if (status != ESDM_SUCCESS) {
      return NC_EINVAL;
    }

    esdm_dataspace_t *dspace;
    status = esdm_dataset_get_dataspace(dset, &dspace);
    if (status != ESDM_SUCCESS){
      return NC_EACCESS;
    }
    int ndims = esdm_dataspace_get_dims(dspace);
    char const *const *names = NULL;
    status = esdm_dataset_get_name_dims(dset, &names);
    if (status != ESDM_SUCCESS || names == NULL) {
      WARN("the container doesn't include named dimensions!");
      return NC_EINVAL;
    }

    md_var_t *evar = malloc(sizeof(md_var_t));
    evar->dimidsp = malloc(sizeof(int) * ndims);
    evar->dset = dset;
    evar->fillmode = NC_FILL;

    int64_t const *dspace_size = esdm_dataset_get_size(dset);

    for (int j = 0; j < ndims; j++) {
      // check if the dim already exists in the dim table
      int dim_found = -1;
      for (int k = 0; k < e->dimt.count; k++) {
        if (strcmp(e->dimt.name[k], names[j]) == 0) {
          // found it!
          if (e->dimt.size[k] != dspace_size[j]) {
            WARN("Dimensions are not matching for %s", names[j]);
            return NC_EINVAL;
          }
          dim_found = k;
          break;
        }
      }
      if (dim_found == -1) {
        dim_found = e->dimt.count;
        add_to_dims_tbl(e, names[j], dspace_size[j]);
      }
      evar->dimidsp[j] = dim_found;
    }
    insert_md(&e->vars, evar);
  }

  smd_attr_t *attrs;
  status = esdm_container_get_attributes(e->c, &attrs);
  int dims_pos = smd_find_position_by_name(attrs, "_nc_dims");
  int sizes_pos = smd_find_position_by_name(attrs, "_nc_sizes");
  if (dims_pos >= 0 || sizes_pos >= 0) {
    if (dims_pos >= 0 && sizes_pos >= 0) {
      e->original_nc = 1;
      smd_attr_t *dims = smd_attr_get_child(attrs, dims_pos);
      smd_attr_t *sizes = smd_attr_get_child(attrs, sizes_pos);

      uint64_t count = smd_attr_elems(dims);
      if (smd_attr_elems(sizes) != count) {
        WARN("stored dimensions and sizes do not match, will ignore them");
      } else {
        char const **names = smd_attr_get_value(dims);
        uint64_t *dim_sizes = smd_attr_get_value(sizes);
        for (uint64_t i = 0; i < count; i++) {
          int dim_found = -1;
          for (int k = 0; k < e->dimt.count; k++) {
            if (strcmp(e->dimt.name[k], names[i]) == 0) {
              dim_found = k;
              break;
            }
          }
          if (dim_found == -1) {
            dim_found = e->dimt.count;
            // WARN("Adding unused dim: %s %lld", names[i], dim_sizes[i]);
            add_to_dims_tbl(e, names[i], dim_sizes[i]);
          }
        }
      }
    } else {
      WARN("stored only dimensions or sizes, will ignore them");
    }
    ncesdm_remove_attr(c);
  }

  return NC_NOERR;
}

/**
 * @brief Put open netcdf dataset into define mode.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 */

// TODO

int ESDM_redef(int ncid) {
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

/**
 * @brief Leave define mode with performance tuning.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param h_minfree	Sets the pad at the end of the "header" section.
 * @param v_align	Controls the alignment of the beginning of the data section for fixed size variables.
 * @param v_minfree	Sets the pad at the end of the data section for fixed size variables.
 * @param r_align	Controls the alignment of the beginning of the data section for variables which have an unlimited dimension (record variables).
 */

int ESDM__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align) {
  DEBUG_ENTER("%d\n", ncid);
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EACCESS;

  int ret = ncesdm_container_commit(e);

  ncesdm_remove_attr(e->c);
  return ret;
}

/**
 * @brief Synchronize an open netcdf dataset to disk. The function nc_sync()
 * offers a way to synchronize the disk copy of a netCDF dataset with in-memory
 * buffers.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 */

// I don't think we are dealing with this yet.

int ESDM_sync(int ncid) {
  DEBUG_ENTER("%d\n", ncid);
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EACCESS;

  int ret = ncesdm_container_commit(e);
  return ret;
}

/**
 * @brief No longer necessary for user to invoke manually. The function
 * nc_abort() just closes the netCDF dataset, if not in define mode
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 */

// TODO

int ESDM_abort(int ncid) {
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

/**
 * @brief Close an open netCDF dataset.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 */

int ESDM_close(int ncid, void *b) {
  DEBUG_ENTER("%d\n", ncid);
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  int ret = ncesdm_container_commit(e);

  int ndsets = esdm_container_dataset_count(e->c);
  // close all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++) {
    esdm_dataset_t *dset = esdm_container_dataset_from_array(e->c, i);
    ret = esdm_dataset_close(dset);
  }
  ret = esdm_container_close(e->c);

  // TODO CLOSE the container properly
  free(e);
  return ret;
}

/**
 * @brief Change the fill-value mode to improve write performance.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param fillmode	Desired fill mode for the dataset, either NC_NOFILL or
 * NC_FILL.
 * @param old_modep	Pointer to location for returned current fill mode of
 * the dataset before this call, either NC_NOFILL or NC_FILL.
 */

int ESDM_set_fill(int ncid, int fillmode, int *old_modep) {
  DEBUG_ENTER("%d %d\n", ncid, fillmode);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;
  if (old_modep) {
    *old_modep = e->fillmode;
  }
  e->fillmode = fillmode;

  return NC_NOERR;
}

/**
 * @brief Set the fill value for a variable.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param varid	Variable ID.
 * @param no_fill	Set to NC_NOFILL to turn off fill mode for this
 * variable. Set to NC_FILL (the default) to turn on fill mode for the variable.
 * @param fill_value	the fill value to be used for this variable. Must be the
 * same type as the variable. This must point to enough free memory to hold one
 * element of the data type of the variable. (For example, an NC_INT will
 * require 4 bytes for it's fill value, which is also an NC_INT.)
 */

int ESDM_def_var_fill(int ncid, int varid, int no_fill, const void *fill_value) {
  DEBUG_ENTER("%d %d\n", ncid, no_fill);
  esdm_status status;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;
  if (varid > e->vars.count) {
    return NC_EACCESS;
  }
  md_var_t *ev = e->vars.var[varid];
  ev->fillmode = no_fill;

  if (no_fill || no_fill == NC_NOFILL || fill_value == NULL) { // should be NC_NOFILL, but the NetCDF itself is not following this convention, see: https://www.unidata.ucar.edu/software/netcdf/docs/group__variables.html#gabe75b6aa066e6b10a8cf644fb1c55f83
    set_default_fill_mode(ev->dset);
  } else {
    status = esdm_dataset_set_fill_value(ev->dset, fill_value);
    assert(status == ESDM_SUCCESS);
    esdm_dataspace_t *space;
    status = esdm_dataset_get_dataspace(ev->dset, &space);
    esdm_type_t type = esdm_dataspace_get_type(space);
    smd_attr_t *attr = smd_attr_new("_FillValue", type, fill_value);
    status = esdm_dataset_link_attribute(ev->dset, 1, attr);
  }

  return NC_NOERR;
}

/**
 * @brief This function will change the parallel access of a variable from independent to collective and vice versa.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param varid	Variable ID
 * @param par_access	NC_COLLECTIVE or NC_INDEPENDENT.
 */

int ESDM_var_par_access(int ncid, int varid, int access) { // for parallel execution
  DEBUG_ENTER("%d: var:%d access:%d\n", ncid, varid, access);

  if (access == NC_INDEPENDENT) {
    access = NC_COLLECTIVE;
  } else {
    access == NC_INDEPENDENT;
  }

  return NC_NOERR;
}

/**
 * @brief Inquire about the binary format of a netCDF file as presented by the API.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param formatp	Pointer to location for returned format version, one of NC_FORMAT_CLASSIC, NC_FORMAT_64BIT_OFFSET, NC_FORMAT_CDF5, NC_FORMAT_NETCDF4, NC_FORMAT_NETCDF4_CLASSIC.
 */

int ESDM_inq_format(int ncid, int *formatp) {
  DEBUG_ENTER("%d\n", ncid);
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (!formatp)
    return NC_NOERR;

  *formatp = NC_FORMATX_ESDM;
  return NC_NOERR;
}

/**
 * @brief Obtain more detailed (vis-a-vis nc_inq_format) format information
 * about an open dataset.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param formatp	Pointer to location for returned true format.
 * @param modep	Pointer to location for returned mode flags.
 */

// TODO

int ESDM_inq_format_extended(int ncid, int *formatp, int *modep) {
  DEBUG_ENTER("%d\n", ncid);
  *formatp = NC_FORMATX_ESDM;
  *modep = NC_ESDM;
  return NC_NOERR;
}

/**
 * @brief Inquire about a file or group.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param formatp	Pointer to location for returned true format.
 * @param modep	Pointer to location for returned mode flags.
 * @param ndimsp	Pointer to location for returned number of dimensions
 * defined for this netCDF dataset. Ignored if NULL.
 * @param nvarsp	Pointer to location for returned number of variables
 * defined for this netCDF dataset. Ignored if NULL.
 * @param nattsp	Pointer to location for returned number of global
 * attributes defined for this netCDF dataset. Ignored if NULL.
 * @param unlimdimidp	Pointer to location for returned ID of the unlimited
 * dimension, if there is one for this netCDF dataset. If no unlimited length
 * dimension has been defined, -1 is returned. Ignored if NULL. If there are
 * multiple unlimited dimensions (possible only for netCDF-4 files), only a
 * pointer to the first is returned, for backward compatibility. If you want
 * them all, use nc_inq_unlimids().
 */

int ESDM_inq(int ncid, int *ndimsp, int *nvarsp, int *nattsp, int *unlimdimidp) {
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (nattsp) {
    smd_attr_t *attr;
    status = esdm_container_get_attributes(e->c, &attr);
    if (status != ESDM_SUCCESS) {
      return NC_EACCESS;
    }
    *nattsp = smd_attr_count(attr);
  }

  if (ndimsp) {
    *ndimsp = e->dimt.count;
  }

  if (nvarsp) {
    int count = esdm_container_dataset_count(e->c);
    *nvarsp = count;
  }

  if (unlimdimidp) {
    *unlimdimidp = -1;
    for (int i = 0; i < e->dimt.count; i++) {
      if (e->dimt.size[i] == 0) {
        *unlimdimidp = i;
        return NC_NOERR;
      }
    }
  }

  return NC_NOERR;
}

/**
 * @brief Inquire about a type.
 * @param ncid	The ncid for the group containing the type (ignored for atomic
 * types).
 * @param xtype	The typeid for this type, as returned by nc_def_compound,
 * nc_def_opaque, nc_def_enum, nc_def_vlen, or nc_inq_var, or as found in
 * netcdf.h in the list of atomic types (NC_CHAR, NC_INT, etc.).
 * @param name	If non-NULL, the name of the user defined type will be copied
 * here. It will be NC_MAX_NAME bytes or less. For atomic types, the type name
 * from CDL will be given.
 * @param size	If non-NULL, the (in-memory) size of the type in bytes will be
 * copied here. VLEN type size is the size of nc_vlen_t. String size is returned
 * as the size of a character pointer. The size may be used to malloc space for
 * the data, no matter what the type.
 */

int ESDM_inq_type(int ncid, nc_type xtype, char *name, size_t *size) {
  DEBUG_ENTER("%d\n", ncid);

  // TODO

  return NC_NOERR;
}

/**
 * @brief Define a new dimension.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param name	Name of the dimension to be created.
 * @param len	Length of the dimension to be created. Use NC_UNLIMITED for unlimited dimensions.
 * @param idp	Pointer where dimension ID will be stored.
 */

int ESDM_def_dim(int ncid, const char *name, size_t len, int *idp) {
  DEBUG_ENTER("%d: %s\n", ncid, name);
  int ret = NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  // ensure that the name hasn't been defined if it was defined, replace it
  for (int i = 0; i < e->dimt.count; i++) {
    if (strcmp(e->dimt.name[i], name) == 0) {
      e->dimt.size[i] = len;
      return ret;
    }
  }

  int cnt = e->dimt.count;
  if (idp) {
    *idp = cnt;
    DEBUG_ENTER("%d: %d\n", ncid, *idp);
  }

  add_to_dims_tbl(e, name, len);

  return NC_NOERR;
}

/**
 * @brief Find the ID of a dimension from the name.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param name	Name of the dimension.
 * @param idp	Pointer where dimension ID will be stored.
 */

int ESDM_inq_dimid(int ncid, const char *name, int *idp) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (name == NULL)
    return NC_EACCESS;

  for (int i = 0; i < e->dimt.count; i++) {
    if (strcmp(e->dimt.name[i], name) == 0) {
      *idp = i;
      return NC_NOERR;
    }
  }

  return NC_EBADID;
}

/**
 * @brief Find the name and length of a dimension. The length for the unlimited
 * dimension, if any, is the number of records written so far.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param dimid	Dimension ID, from a previous call to nc_inq_dimid() or
 * nc_def_dim().
 * @param name	Returned dimension name. The caller must allocate space for the
 * returned name. The maximum possible length, in characters, of a dimension
 * name is given by the predefined constant NC_MAX_NAME. (This doesn't include
 * the null terminator, so declare your array to be size NC_MAX_NAME+1). The
 * returned character array will be null-terminated.
 * @param lenp	Pointer to location for returned length of dimension. For the
 * unlimited dimension, this is the number of records written so far.
 */

int ESDM_inq_dim(int ncid, int dimid, char *name, size_t *lenp) {
  DEBUG_ENTER("%d %d\n", ncid, dimid);
  int ret = NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  assert(e->dimt.count > dimid);

  if (name) {
    strcpy(name, e->dimt.name[dimid]);
    DEBUG_ENTER("%s\n", name);
  }

  if (lenp) {
    size_t size = e->dimt.size[dimid];
    if (size == 0) {
      size = esdm_container_dataset_get_actual_size(e, dimid);
    }
    *lenp = size;
  }

  return NC_NOERR;
}

/**
 * @brief Find the ID of the unlimited dimension.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param unlimdimidp	Pointer where unlimited dimension ID will be stored. If
 * there is no unlimited dimension, -1 will be stored here. Ignored if NULL.
 */

int ESDM_inq_unlimdim(int ncid, int *unlimdimidp) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (unlimdimidp) {
    *unlimdimidp = -1;
    for (int i = 0; i < e->dimt.count; i++) {
      if (e->dimt.size[i] == 0) {
        *unlimdimidp = i;
        return NC_NOERR;
      }
    }
  }

  return NC_NOERR;
}

/**
 * @brief Rename a dimension.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param dimid	Dimension ID, from a previous call to nc_inq_dimid() or
 * nc_def_dim().
 * @param name	New name for dimension. Must be a null-terminated string with
 * length less than NC_MAX_NAME.
 */

// we may have sealed containers, that won't allow rename
// hot to control if the file was opened as read/write here?

int ESDM_rename_dim(int ncid, int dimid, const char *name) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;
  if (name == NULL)
    return NC_EACCESS;

  if (dimid >= e->dimt.count)
    return NC_EINVAL;

  // Check if the name is taken

  for (int i = 0; i < e->dimt.count; i++) {
    if (strcmp(e->dimt.name[i], name) == 0) {
      return NC_EACCESS;
    }
  }

  esdm_status status;
  esdm_container_t *c = e->c;
  int ndsets = esdm_container_dataset_count(c);
  // open all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++) {
    esdm_dataset_t *dset = esdm_container_dataset_from_array(c, i);
    if (dset == NULL)
      return NC_EACCESS;
    char const *const *names;
    status = esdm_dataset_get_name_dims(dset, &names);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
    if (names == NULL)
      return NC_EACCESS;

    // find the dimensions and the position the specific dimension has to change

    esdm_dataspace_t *space;
    status = esdm_dataset_get_dataspace(dset, &space);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;

    int64_t ndims = esdm_dataspace_get_dims(space);
    for (int j = 0; j < ndims; j++) {
      if (strcmp(names[j], e->dimt.name[dimid]) == 0) {
        status = esdm_dataset_rename_dim(dset, name, j);
        if (status != ESDM_SUCCESS)
          return NC_EACCESS;
      }
    }
  }

  free(e->dimt.name[dimid]);
  e->dimt.name[dimid] = strdup(name);
  // d->container->status = ESDM_DATA_DIRTY;

  return NC_NOERR;
}

/**
 * @brief Return information about a netCDF attribute. The function nc_inq_att
 * returns the attribute's type and length.
 * @paramncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @paramvarid	Variable ID of the attribute's variable, or NC_GLOBAL for a
 * global attribute.
 * @paramname	Pointer to the location for the returned attribute NetCDF Names.
 * Ignored if NULL.
 * @paramxtypep	Pointer to location for returned attribute Data Types. Ignored
 * if NULL.
 * @paramlenp	Pointer to location for returned number of values currently
 * stored in the attribute. For attributes of type NC_CHAR, you should not
 * assume that this includes a trailing zero byte; it doesn't if the attribute
 * was stored without a trailing zero byte, for example from a FORTRAN program.
 * Before using the value as a C string, make sure it is null-terminated.
 * Ignored if NULL.
 */

int ESDM_inq_att(int ncid, int varid, const char *name, nc_type *datatypep, size_t *lenp) {
  DEBUG_ENTER("%d name:%s\n", ncid, name);

  esdm_status status;

  if (name == NULL)
    return NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;
  if (varid == NC_GLOBAL) {
    status = esdm_container_get_attributes(e->c, &attr);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, &attr);
    if (status != ESDM_SUCCESS) {
      return NC_EACCESS;
    }
  }

  smd_attr_t *a;
  a = smd_attr_get_child_by_name(attr, name);
  if (!a) {
    return NC_ENOTATT;
  }

  if (datatypep) {
    if (a->type->type == SMD_TYPE_ARRAY) {
      *datatypep = type_esdm_to_nc(a->type->specifier.u.arr.base->type);
    } else {
      *datatypep = type_esdm_to_nc(a->type->type);
      if(*datatypep == NC_STRING){
        // workaround, allows to store strings with ESDM and read them back as NetCDF files
        *datatypep = NC_CHAR;
      }
    }
  }

  if (lenp) {
    if(a->type->type == SMD_TYPE_STRING){
      // workaround, allows to store strings with ESDM and read them back as NetCDF files
      *lenp = strlen((char*) a->value);
    }else{
      *lenp = smd_attr_elems(a);
    }
  }

  return NC_NOERR;
}

/**
 * @brief Find an attribute ID.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @param name	Attribute NetCDF Names.
 * @param idp	Pointer to location for returned attribute number that specifies which attribute this is for this variable (or which global attribute). If you already know the attribute name, knowing its number is not very useful, because accessing information about an attribute requires its name.
 */

int ESDM_inq_attid(int ncid, int varid, const char *name, int *attnump) {
  DEBUG_ENTER("%d\n", ncid);
  esdm_status status;

  if (attnump == NULL)
    return NC_NOERR;

  if (name == NULL)
    return NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;
  if (varid == NC_GLOBAL) {
    status = esdm_container_get_attributes(e->c, &attr);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, &attr);
    if (status != ESDM_SUCCESS) {
      return NC_EACCESS;
    }
  }

  for (int i = 0; i < attr->children; i++) {
    if (strcmp(name, attr->childs[i]->name) == 0) {
      *attnump = i;
      // *attnump = attr->childs[i]->id;

      // The return for attnump is i or attr->childs[i]->id?!

      return NC_NOERR;
    }
  }
}

/**
 * @brief Find the name of an attribute.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @param attnum	Attribute number. The attributes for each variable are numbered from 0 (the first attribute) to natts-1, where natts is the number of attributes for the variable, as returned from a call to nc_inq_varnatts().
 * @param name	Pointer to the location for the returned attribute NetCDF Names.
 */

int ESDM_inq_attname(int ncid, int varid, int attnum, char *name) {
  DEBUG_ENTER("%d\n", ncid);

  if (name == NULL)
    return NC_NOERR;

  assert(attnum >= 0);
  esdm_status status;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;
  if (varid == NC_GLOBAL) {
    status = esdm_container_get_attributes(e->c, &attr);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, &attr);
    if (status != ESDM_SUCCESS) {
      return NC_EACCESS;
    }
  }

  strcpy(name, attr->childs[attnum]->name);

  return NC_NOERR;
}

/**
 * @brief Rename an attribute.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @param name	Attribute NetCDF Names.
 * @param newname	The new attribute NetCDF Names.
 */

// We may have sealed containers, that won't allow rename

int ESDM_rename_att(int ncid, int varid, const char *name, const char *newname) {
  DEBUG_ENTER("%d\n", ncid);
  esdm_status status;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;
  md_var_t *ev;
  if (varid == NC_GLOBAL) {
    status = esdm_container_get_attributes(e->c, &attr);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, &attr);
    if (status != ESDM_SUCCESS) {
      return NC_EACCESS;
    }
  }

  int attnum;
  status = ESDM_inq_attid(ncid, varid, name, &attnum);
  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  free((void *)attr->childs[attnum]->name);
  attr->childs[attnum]->name = strdup(newname);

  if (varid == NC_GLOBAL) {
    esdm_container_set_status_dirty(e->c);
  } else {
    esdm_dataset_set_status_dirty(ev->dset);
  }

  return NC_NOERR;
}

/**
 * @brief Delete an attribute. The function nc_del_att() deletes a netCDF
 * attribute from an open netCDF dataset. The netCDF dataset must be in define
 * mode.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a
 * global attribute.
 * @param name	Attribute name. */

int ESDM_del_att(int ncid, int varid, const char *name) {
  DEBUG_ENTER("%d\n", ncid);
  esdm_status status;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (varid == NC_GLOBAL) {
    status = esdm_container_delete_attribute(e->c, name);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_delete_attribute(ev->dset, name);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  }

  return NC_NOERR;
}

/**
 * @brief Get an attribute of any type.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @param name	Attribute name.
 * @param value	Pointer to location for returned attribute value(s). All elements of the vector of attribute values are returned, so you must allocate enough space to hold them. Before using the value as a C string, make sure it is null-terminated. Call nc_inq_attlen() first to find out the length of the attribute.
 */

int ESDM_get_att(int ncid, int varid, const char *name, void *value, nc_type type) {
  DEBUG_ENTER("%d\n", ncid);

  if (name == NULL) {
    return NC_EACCESS;
  }
  esdm_status status;
  int ret;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *att;
  if (varid == NC_GLOBAL) {
    status = esdm_container_get_attributes(e->c, &att);
    if (status != ESDM_SUCCESS)
      return NC_EACCESS;
  } else {
    if (varid > e->vars.count) {
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, &att);
    if (status != ESDM_SUCCESS) {
      smd_attr_destroy(att);
      return NC_EACCESS;
    }
  }

  smd_attr_t *child = smd_attr_get_child_by_name(att, name);
  if (child != NULL) {
    if (child->type->type == SMD_TYPE_STRING) {
      char const *str;
      smd_attr_copy_value(child, &str);
      strcpy(value, str);
    } else {
      smd_attr_copy_value_usertype(child, type_nc_to_esdm(type), value);
    }
    return NC_NOERR;
  }

  return NC_ENOTATT;
}

/**
 * @brief Write an attribute.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the variable to which the attribute will be assigned or NC_GLOBAL for a global or group attribute.
 * @param name	Attribute NetCDF Names. Appendix A: Attribute Conventions may apply.
 * @param xtype	Data Types of the attribute.
 * @param len	Number of values provided for the attribute.
 * @param value	Pointer to one or more values.
 */

int ESDM_put_att(int ncid, int varid, const char *name, nc_type datatype, size_t len, void const *value, nc_type type) {
  DEBUG_ENTER("%d\n", ncid);

  esdm_type_t etype;

  etype = type_nc_to_esdm(datatype);
  if (etype == NULL) {
    return NC_EACCESS;
  }

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (strcmp(name, "_FillValue") == 0) {
    // special case as NetCDF allows to manipulate an attribute with that name.
    if (len != 1 || varid == NC_GLOBAL) {
      return NC_EINVAL;
    }
    esdm_dataspace_t *space;
    if (varid > esdm_container_dataset_count(e->c)) {
      return NC_EBADID;
    }
    md_var_t *ev = e->vars.var[varid];
    esdm_status status = esdm_dataset_get_dataspace(ev->dset, &space);
    nc_type dset_datatype = type_esdm_to_nc(esdm_dataspace_get_type(space)->type);
    if (dset_datatype != datatype && datatype != NC_NAT) {
      return NC_EBADTYPE;
    }
    status = esdm_dataset_set_fill_value(ev->dset, value);
    if (status != ESDM_SUCCESS) {
      return NC_EINVAL;
    }
  }

  smd_attr_t *new;
  if (len > 1) {
    smd_dtype_t *arr_type = smd_type_array(etype, len);
    new = smd_attr_new(name, arr_type, value);
    // smd_type_unref(arr_type);
  } else {
    if (datatype == NC_STRING) {
      new = smd_attr_new(name, etype, *(void **)value);
    } else if (etype == SMD_DTYPE_STRING) {
      new = smd_attr_new(name, etype, value);
    } else {
      new = smd_attr_new_usertype(name, type_nc_to_esdm(type), etype, value);
    }
    if (!new) {
      return NC_EACCESS;
    }
  }

  // if (type != datatype) {
  //   free(value);
  // }

  esdm_status status;

  if (varid == NC_GLOBAL) {
    status = esdm_container_link_attribute(e->c, 1, new);
  } else {
    if (varid > esdm_container_dataset_count(e->c)) {
      smd_attr_destroy(new);
      return NC_EACCESS;
    }
    md_var_t *ev = e->vars.var[varid];
    status = esdm_dataset_link_attribute(ev->dset, 1, new);
  }
  if (status != ESDM_SUCCESS) {
    smd_attr_destroy(new);
    return NC_EACCESS;
  }
  return NC_NOERR;
  // return NC_ESTRICTNC3;
}

/**
 * @brief
 * @param
 */

int ESDM_def_var(int ncid, const char *name, nc_type xtype, int ndims, const int *dimidsp, int *varidp) {
  DEBUG_ENTER("%d %s\n", ncid, name);
  int ret = NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (varidp) {
    *varidp = e->vars.count;
    DEBUG_ENTER("%d: varid: %d\n", ncid, *varidp);
  }

  md_var_t *evar = malloc(sizeof(md_var_t));
  evar->fillmode = NC_FILL;
  char *names[ndims];
  int64_t bounds[ndims];

  if (dimidsp) {
    evar->dimidsp = malloc(sizeof(int) * ndims);

    for (int i = 0; i < ndims; i++) {
      int dimid = dimidsp[i];
      assert(e->dimt.count > dimid);

      size_t val = e->dimt.size[dimid];
      names[i] = e->dimt.name[dimid];
      bounds[i] = val;
      evar->dimidsp[i] = dimid;
    }
  }

  esdm_type_t typ = type_nc_to_esdm(xtype);
  if (typ == NULL || typ == SMD_DTYPE_UNKNOWN || typ == SMD_DTYPE_STRING) {
    return NC_EBADTYPE;
  }

  esdm_dataspace_t *dataspace;
  esdm_status status = esdm_dataspace_create(ndims, bounds, typ, &dataspace);
  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  esdm_dataset_t *d;

#ifdef ESDM_PARALLEL
  if (e->parallel_mode) {
    status = esdm_mpi_dataset_create(e->comm, e->c, name, dataspace, &d);
  } else {
    status = esdm_dataset_create(e->c, name, dataspace, &d);
  }
#else
  status = esdm_dataset_create(e->c, name, dataspace, &d);
#endif

  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  status = esdm_dataset_name_dims(d, names);
  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  set_default_fill_mode(d);

  evar->dset = d;
  insert_md(&e->vars, evar);

  return NC_NOERR;
}

/**
 * @brief Find the ID of a variable, from the name.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param name	Name of the variable.
 * @param varidp	Pointer to location for returned variable ID. Ignored if
 * NULL.
 */

int ESDM_inq_varid(int ncid, const char *name, int *varidp) {
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;

  if (name == NULL)
    return NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;

  esdm_container_t *c = e->c;
  int ndsets = esdm_container_dataset_count(c);
  // open all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++) {
    esdm_dataset_t *dset = esdm_container_dataset_from_array(c, i);
    if (dset == NULL)
      return NC_EACCESS;

    char const *dname = esdm_dataset_name(dset);
    if (dname == NULL)
      return NC_NOERR;

    if (strcmp(dname, name) == 0) {
      *varidp = i;
      return NC_NOERR;
    }
  }
  return NC_EACCESS;
}

// we may have sealed containers, that won't allow rename

/**
 * @brief Rename a variable.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), or associated inquiry functions such as
 * nc_inq_ncid().
 * @param varid	Variable ID
 * @param name	New name of the variable.
 */

int ESDM_rename_var(int ncid, int varid, const char *name) {
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;

  if (name == NULL)
    return NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  smd_attr_t *attr;
  if (varid > e->vars.count) {
    return NC_EINVAL;
  }
  md_var_t *ev = e->vars.var[varid];
  assert(ev != NULL);

  // TODO test to avoid using the same name

  status = esdm_dataset_rename(ev->dset, name);
  if (status != ESDM_SUCCESS)
    return NC_EACCESS;

  return NC_NOERR;
}

/**
 * @brief
 * @param
 */

int ESDM_get_vars(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type) {
  DEBUG_ENTER("%d %d\n", ncid, varid);
  int ret_NC = NC_NOERR;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  assert(e->vars.count > varid);
  md_var_t *kv = e->vars.var[varid];
  DEBUG_ENTER("%d %d type: %d buff: %p %p %p %p\n", ncid, varid, mem_nc_type, data,
  startp, countp, stridep);

  // check the dimensions we actually want to write
  esdm_dataspace_t *space;
  esdm_status status = esdm_dataset_get_dataspace(kv->dset, &space);

  if (mem_nc_type != type_esdm_to_nc(esdm_dataspace_get_type(space)->type) && mem_nc_type != NC_NAT) {
    // TODO stridep

  }
  int ndims = esdm_dataspace_get_dims(space);
  int64_t const *spacesize = esdm_dataset_get_actual_size(kv->dset);

  int64_t size[ndims];
  int64_t offset[ndims];
  for (int i = 0; i < ndims; i++) {
    size[i] = countp[i];
    offset[i] = startp[i];
  }
  esdm_dataspace_t *subspace;
  status = esdm_dataspace_subspace(space, ndims, size, offset, &subspace);
  if (status != ESDM_SUCCESS) {
    int count = 0;
    for (int i = 0; i < ndims; i++)
      if (size[i] == 0) {
        count++;
      }
    if (count != 0) {
      return NC_NOERR;
    } else {
      return NC_EACCESS;
    }
  }
  status = esdm_read(kv->dset, (void *)data, subspace);
  esdm_dataspace_destroy(subspace);
  if (status != ESDM_SUCCESS) {
    return NC_EINVAL;
  }

  return NC_NOERR;
}

/**
 * @brief Read an array of values from a variable.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID
 * @param startp	Start vector with one element for each dimension to Specify a Hyperslab.
 * @param countp	Count vector with one element for each dimension to Specify a Hyperslab.
 * @param ip	Pointer where the data will be copied. Memory must be allocated by the user before this function is called.
 */

int ESDM_get_vara(int ncid, int varid, const size_t *startp, const size_t *countp, void *ip, int memtype) {
  DEBUG_ENTER("%d %d\n", ncid, varid);
  return ESDM_get_vars(ncid, varid, startp, countp, NULL, ip, memtype);
}

/**
 * @brief Write a strided array of values to a variable.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID
 * @param startp	Start vector with one element for each dimension to Specify a Hyperslab.
 * @param countp	Count vector with one element for each dimension to Specify a Hyperslab.
 * @param stridep	Stride vector with one element for each dimension to Specify a Hyperslab.
 * @param op	Pointer where the data will be copied. Memory must be allocated by the user before this function is called.
 */

int ESDM_put_vars(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type) {
  DEBUG_ENTER("%d %d\n", ncid, varid);
  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  assert(e->vars.count > varid);
  md_var_t *kv = e->vars.var[varid];
  DEBUG_ENTER("%d %d type: %d buff: %p %p %p %p\n", ncid, varid, mem_nc_type, data, startp, countp, stridep);

  // check the dimensions we actually want to write
  esdm_dataspace_t *space;
  esdm_status status = esdm_dataset_get_dataspace(kv->dset, &space);
  if (status != ESDM_SUCCESS) {
    return NC_EACCESS;
  }

  nc_type datatype = type_esdm_to_nc(esdm_dataspace_get_type(space)->type);
  if (mem_nc_type != datatype && mem_nc_type != NC_NAT) {
    DEBUG("memory data type differs from file datatatype");
    DEBUG("Mem: %d Data: %d\n", mem_nc_type, datatype);
    // TODO stridep
  }

  int ndims = esdm_dataspace_get_dims(space);
  int64_t const *spacesize = esdm_dataset_get_actual_size(kv->dset);

  int64_t size[ndims];
  int64_t offset[ndims];
  for (int i = 0; i < ndims; i++) {
    size[i] = countp[i];
    offset[i] = startp[i];
  }
  esdm_dataspace_t *subspace;
  status = esdm_dataspace_subspace(space, ndims, size, offset, &subspace);
  if (status != ESDM_SUCCESS) {
    int count = 0;
    for (int i = 0; i < ndims; i++)
      if (size[i] == 0) {
        count++;
      }

    if (count != 0) {
      return NC_NOERR;
    } else {
      return NC_EACCESS;
    }
  }

  status = esdm_write(kv->dset, (void *)data, subspace);

  esdm_dataspace_destroy(subspace);
  if (status != ESDM_SUCCESS) {
    return NC_EINVAL;
  }

  return NC_NOERR;
}

/**
 * @brief Write an array of values to a variable.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID
 * @param startp	Start vector with one element for each dimension to Specify a Hyperslab.
 * @param countp	Count vector with one element for each dimension to Specify a Hyperslab.
 * @param op	Pointer where the data will be copied. Memory must be allocated by the user before this function is called.
 */

int ESDM_put_vara(int ncid, int varid, const size_t *startp, const size_t *countp, const void *op, int memtype) {
  DEBUG_ENTER("%d\n", ncid);
  return ESDM_put_vars(ncid, varid, startp, countp, NULL, op, memtype);
}

/**
* @brief Learn all about a variable.
* @param 	ncid	ncid for file.
* @param 	varid	varid for variable in question.
* @param 	name	Pointer to memory to contain the name of the variable.
* @param 	xtypep	Pointer to memory to contain the type of the variable.
* @param 	ndimsp	Pointer to memory to store the number of associated
dimensions for the variable.
* @param 	dimidsp	Pointer to memory to store the dimids associated with
the variable.
* @param 	nattsp	Pointer to memory to store the number of attributes
associated with the variable.
* @param 	no_fill	Pointer to memory to store whether or not there is a
fill value associated with the variable.
* @param 	fill_valuep	Pointer to memory to store the fill value (if
one exists) for the variable.
* @param 	contiguousp	Pointer to memory to store contiguous-data
information associated with the variable.
* @param 	endiannessp	Pointer to memory to store endianness value. One
of NC_ENDIAN_BIG NC_ENDIAN_LITTLE NC_ENDIAN_NATIVE
// About compression, not supported.
* @param 	shufflep	Pointer to memory to store shuffle information
associated with the variable.
* @param 	deflatep	Pointer to memory to store compression type
associated with the variable.
* @param 	deflate_levelp	Pointer to memory to store compression level
associated with the variable.
* @param 	fletcher32p	Pointer to memory to store compression
information associated with the variable.
* @param 	chunksizesp	Pointer to memory to store chunksize information
associated with the variable.
* @param 	idp	Pointer to memory to store filter id.
* @param 	nparamsp	Pointer to memory to store filter parameter
count.
* @param 	params	Pointer to vector of unsigned integers into which to
store filter parameters.
*/

int ESDM_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, int *ndimsp, int *dimidsp, int *nattsp, int *shufflep, int *deflatep, int *deflate_levelp, int *fletcher32p, int *contiguousp, size_t *chunksizesp, int *no_fill, void *fill_valuep, int *endiannessp, unsigned int *idp, size_t *nparamsp, unsigned int *params) {
  DEBUG_ENTER("%d %d\n", ncid, varid);
  esdm_status status;

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (varid == NC_GLOBAL) {
    if (nattsp) { // the number of attributes
      smd_attr_t *attr = NULL;
      status = esdm_container_get_attributes(e->c, &attr);
      assert(status == ESDM_SUCCESS);
      *nattsp = smd_attr_count(attr);
    }
    return NC_NOERR;
  }else{
    if (varid >= e->vars.count){
      return NC_EINVAL;
    }
  }

  md_var_t *evar = e->vars.var[varid];
  assert(evar != NULL);

  esdm_dataspace_t *space;
  status = esdm_dataset_get_dataspace(evar->dset, &space);
  assert(status == ESDM_SUCCESS);

  if (name) {
    strcpy(name, esdm_dataset_name(evar->dset));
  }

  if (xtypep) {
    *xtypep = type_esdm_to_nc(esdm_dataspace_get_type(space)->type);
  }

  if (ndimsp) {
    *ndimsp = esdm_dataspace_get_dims(space);
  }

  if (dimidsp) {
    int ndims = esdm_dataspace_get_dims(space);
    for (int i = 0; i < ndims; i++) {
      dimidsp[i] = evar->dimidsp[i];
    }
  }

  if (nattsp) { // the number of attributes
    smd_attr_t *attr = NULL;
    status = esdm_dataset_get_attributes(evar->dset, &attr);
    assert(status == ESDM_SUCCESS);
    *nattsp = smd_attr_count(attr);
  }

  if (no_fill) {
    *no_fill = evar->fillmode;
  }

  if (fill_valuep) {
    status = esdm_dataset_get_fill_value(evar->dset, fill_valuep);
    //assert(status == ESDM_SUCCESS);
  }

  if (endiannessp) {
    *endiannessp = NC_ENDIAN_NATIVE;
    //WARN_NOT_SUPPORTED_ENDIAN;
  }

  if (shufflep) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (deflatep) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (deflate_levelp) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (fletcher32p) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (contiguousp) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (chunksizesp) {
    WARN_NOT_SUPPORTED_COMPRESSION;
  }

  if (idp) {
    *idp = 0;
    WARN_NOT_SUPPORTED_FILTER;
  }

  if (nparamsp) {
    *nparamsp = 0;
    WARN_NOT_SUPPORTED_FILTER;
  }

  if (params) {
    *params = 0;
    WARN_NOT_SUPPORTED_FILTER;
  }

  return NC_NOERR;
}

/**
 * @brief Retrieve a list of types associated with a group.
 * @param  	ncid	The ncid for the group in question.
 * @param  	ntypes	Pointer to memory to hold the number of typeids
 * contained by the group in question.
 * @param  	typeids	Pointer to memory to hold the typeids contained by the
 * group in question.
 */

// TO VERIFY

int ESDM_inq_typeids(int ncid, int *ntypes, int *typeids) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL) {
    return NC_EBADID;
  }

  if (ntypes) {
    *ntypes = 0; //SMD_TYPE_STRING;
  }

  if (typeids) {
    for (int i = 0; i < SMD_TYPE_PRIMITIVE_END; i++) {
      typeids[i] = type_esdm_to_nc(i);
    }
  }

  return NC_NOERR;
}

/**
 * @brief Print the metadata for a file.
 * @param ncid	The ncid of an open file.
 */

int ESDM_show_metadata(int ncid) {
  DEBUG_ENTER("%d\n", ncid);
  printf("\nESDM Dataset\n");
  return NC_NOERR;
}

/**
 * @brief Return number and list of unlimited dimensions.
 * @param ncid	NetCDF file and group ID, from a previous call to nc_open(),
 * nc_create(), nc_def_grp(), etc.
 * @param nunlimdimsp	A pointer to an int which will get the number of visible
 * unlimited dimensions. Ignored if NULL.
 * @param unlimdimidsp	A pointer to an already allocated array of int which
 * will get the ids of all visible unlimited dimensions. Ignored if NULL. To
 * allocate the correct length for this array, call nc_inq_unlimdims with a NULL
 * for this parameter and use the nunlimdimsp parameter to get the number of
 * visible unlimited dimensions.
 */

int ESDM_inq_unlimdims(int ncid, int *nunlimdimsp, int *unlimdimidsp) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (unlimdimidsp) {
    int ndims = 0;
    for (int i = 0; i < e->dimt.count; i++) {
      if (e->dimt.size[i] == 0) {
        unlimdimidsp[ndims++] = i;
      }
    }
  }

  if (nunlimdimsp) {
    int ndims = 0;
    for (int i = 0; i < e->dimt.count; i++) {
      if (e->dimt.size[i] == 0) {
        ndims++;
      }
    }
    *nunlimdimsp = ndims;
  }

  return NC_NOERR;
}

/**
 * @brief Find a type by name.
 * @param ncid	NetCDF ID
 * @param name	NetCDF Names of type to search for.
 * @param typeidp	Typeid of named type will be copied here, if it is found.
 */

 // TO FIX

int ESDM_inq_typeid(int ncid, const char *name, nc_type *typeidp) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (typeidp) {
    //*typeidp = type_esdm_to_nc(name);
    // TODO
  }

  return NC_NOERR;

}

/**
 * @brief Return the group ID for a group given the name.
 * @param_in	ncid	A valid file or group ncid.
 * @param_in	name	The name of the group you are querying.
 * @param_out	grp_ncid	Pointer to memory to hold the group ncid.
 */

// TODO

int ESDM_inq_ncid(int ncid, const char *name, int *grp_ncid) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_IMPLEMENTED;
  // Function using groups

  return NC_NOERR;
}

/**
 * @brief Get a list of varids associated with a group given a group ID.
 * @param	ncid	The ncid of the group in question.
 * @param	nvars	Pointer to memory to hold the number of variables in the
 * group in question.
 * @param	varids	Pointer to memory to hold the variable ids contained by
 * the group in question.
 */

int ESDM_inq_varids(int ncid, int *nvars, int *varids) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (nvars)
    *nvars = e->vars.count;

  if (varids) {
    for (int i = 0; i < *nvars; i++) {
      varids[i] = i;
    }
  }

  return NC_NOERR;
}

/**
 * @brief Retrieve a list of dimension ids associated with a group.
 * @param 	ncid	The ncid of the group in question.
 * @param 	ndims	Pointer to memory to contain the number of dimids
 * associated with the group.
 * @param 	dimids	Pointer to memory to contain the number of dimensions
 * associated with the group.
 * @param 	include_parents	If non-zero, parent groups are also traversed.
 */

int ESDM_inq_dimids(int ncid, int *ndims, int *dimids, int include_parents) {
  DEBUG_ENTER("%d\n", ncid);

  // if (include_parents){
  //   WARN_NOT_SUPPORTED_GROUPS;
  //   return NC_EACCESS;
  // }

  nc_esdm_t *e = ESDM_nc_get_esdm_struct(ncid);
  if (e == NULL)
    return NC_EBADID;

  if (ndims) {
    *ndims = e->dimt.count;
  }

  if (dimids) {
    for (int i = 0; i < e->dimt.count; i++) {
      dimids[i] = i;
    }
  }

  // It seems there is not enough information to retrieve the dimension's
  // previous smd_find_position_by_name

  // It's intentional that ESDM doesn't keep the position in which the
  // dimensions were inserted, as long as the variables and their dimensions are
  // coherent. Considering that, the answer seems to be right, but no usufel
  // information is retrieved here.

  return NC_NOERR;
}

// Functions not implemented! Description not found!

/**
 * @brief
 * @param
 */

int ESDM_inq_base_pe(int ncid, int *pe) { // for parallel execution
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_IMPLEMENTED;
  return NC_NOERR;
}

/**
 * @brief
 * @param
 */

int ESDM_set_base_pe(int ncid, int pe) { // for parallel execution
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_IMPLEMENTED;
  return NC_NOERR;
}

// ESDM does not support groups from NetCDF!

int ESDM_inq_grps(int ncid, int *numgrps, int *ncids) {
  DEBUG_ENTER("%d\n", ncid);
  if (numgrps) {
    *numgrps = 0;
  }
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_NOERR;
}

int ESDM_inq_grpname(int ncid, char *name) {
  DEBUG_ENTER("%d\n", ncid);
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_NOERR;
}

int ESDM_inq_grpname_full(int ncid, size_t *lenp, char *full_name) {
  DEBUG_ENTER("%d\n", ncid);
  //WARN_NOT_SUPPORTED_GROUPS;
  if (full_name) {
    strcpy(full_name, "");
  }
  if (lenp) {
    *lenp = 0;
  }
  return NC_NOERR;
}

int ESDM_inq_grp_parent(int ncid, int *parent_ncid) {
  DEBUG_ENTER("%d\n", ncid);
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_ENOGRP;
}

int ESDM_inq_grp_full_ncid(int ncid, const char *full_name, int *grp_ncid) {
  DEBUG_ENTER("%d\n", ncid);
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_NOERR;
}

int ESDM_def_grp(int parent_ncid, const char *name, int *new_ncid) {
  DEBUG_ENTER("%d\n", parent_ncid);
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_NOERR;
}

int ESDM_rename_grp(int grpid, const char *name) {
  DEBUG_ENTER("%d\n", grpid);
  //WARN_NOT_SUPPORTED_GROUPS;
  return NC_NOERR;
}

// ESDM does not support user-defined datatypes from NetCDF!

int ESDM_inq_type_equal(int ncid1, nc_type typeid1, int ncid2, nc_type typeid2, int *equal) {
  DEBUG_ENTER("%d %d\n", ncid1, ncid2);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_inq_user_type(int ncid, nc_type xtype, char *name, size_t *size, nc_type *base_nc_typep, size_t *nfieldsp, int *classp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  if (name) {
    *name = 0;
  }
  if (size) {
    *size = 0;
  }
  return NC_NOERR;
}

int ESDM_def_compound(int ncid, size_t size, const char *name, nc_type *typeidp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_insert_compound(int ncid, nc_type xtype, const char *name, size_t offset, nc_type field_typeid) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_insert_array_compound(int ncid, nc_type xtype, const char *name, size_t offset, nc_type field_typeid, int ndims, const int *dim_sizes) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_inq_compound_field(int ncid, nc_type xtype, int fieldid, char *name, size_t *offsetp, nc_type *field_typeidp, int *ndimsp, int *dim_sizesp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_inq_compound_fieldindex(int ncid, nc_type xtype, const char *name, int *fieldidp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_def_vlen(int ncid, const char *name, nc_type base_typeid, nc_type *xtypep) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_put_vlen_element(int ncid, int typeid1, void *vlen_element, size_t len, const void *data) {
  // DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_get_vlen_element(int ncid, int typeid1, const void *vlen_element, size_t *len, void *data) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_def_enum(int ncid, nc_type base_typeid, const char *name, nc_type *typeidp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_insert_enum(int ncid, nc_type xtype, const char *name, const void *value) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_inq_enum_member(int ncid, nc_type xtype, int idx, char *name, void *value) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_inq_enum_ident(int ncid, nc_type xtype, long long value, char *identifier) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

int ESDM_def_opaque(int ncid, size_t size, const char *name, nc_type *xtypep) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_TYPES;
  return NC_NOERR;
}

// ESDM does not support compression!

int ESDM_def_var_deflate(int ncid, int varid, int shuffle, int deflate, int deflate_level) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_COMPRESSION;
  return NC_NOERR;
}

int ESDM_def_var_fletcher32(int ncid, int varid, int fletcher32) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_COMPRESSION;
  return NC_NOERR;
}

int ESDM_def_var_chunking(int ncid, int varid, int storage, const size_t *chunksizesp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_COMPRESSION;
  return NC_NOERR;
}

int ESDM_set_var_chunk_cache(int ncid, int varid, size_t size, size_t nelems, float preemption) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_COMPRESSION;
  return NC_NOERR;
}

int ESDM_get_var_chunk_cache(int ncid, int varid, size_t *sizep, size_t *nelemsp, float *preemptionp) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_COMPRESSION;
  return NC_NOERR;
}

// ESDM only supports native endianness!

int ESDM_def_var_endian(int ncid, int varid, int endian) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_ENDIAN;
  return NC_NOERR;
}

// ESDM does not support filters!

int ESDM_def_var_filter(int ncid, int varid, unsigned int id, size_t nparams, const unsigned int *parms) {
  DEBUG_ENTER("%d\n", ncid);
  WARN_NOT_SUPPORTED_FILTER;
  return NC_NOERR;
}

static NC_Dispatch esdm_dispatcher = {NC_FORMATX_ESDM,

ESDM_create,
ESDM_open,

ESDM_redef,
ESDM__enddef,
ESDM_sync,
ESDM_abort,
ESDM_close,
ESDM_set_fill,
NC_NOTNC3_inq_base_pe,
NC_NOTNC3_set_base_pe,
ESDM_inq_format,
ESDM_inq_format_extended,

ESDM_inq,
ESDM_inq_type,

ESDM_def_dim,
ESDM_inq_dimid,
ESDM_inq_dim,
ESDM_inq_unlimdim,
ESDM_rename_dim,

ESDM_inq_att,
ESDM_inq_attid,
ESDM_inq_attname,
ESDM_rename_att,
ESDM_del_att,
ESDM_get_att,
ESDM_put_att,

ESDM_def_var,
ESDM_inq_varid,
ESDM_rename_var,
ESDM_get_vara,
ESDM_put_vara,
ESDM_get_vars,
ESDM_put_vars,
NCDEFAULT_get_varm,
NCDEFAULT_put_varm,

ESDM_inq_var_all,
ESDM_var_par_access,
ESDM_def_var_fill,

ESDM_show_metadata,
ESDM_inq_unlimdims,
ESDM_inq_ncid,
ESDM_inq_grps,
ESDM_inq_grpname,
ESDM_inq_grpname_full,
ESDM_inq_grp_parent,
ESDM_inq_grp_full_ncid,
ESDM_inq_varids,
ESDM_inq_dimids,
ESDM_inq_typeids,
ESDM_inq_type_equal,
ESDM_def_grp,
ESDM_rename_grp,
ESDM_inq_user_type,
ESDM_inq_typeid,

ESDM_def_compound,
ESDM_insert_compound,
ESDM_insert_array_compound,
ESDM_inq_compound_field,
ESDM_inq_compound_fieldindex,
ESDM_def_vlen,
ESDM_put_vlen_element,
ESDM_get_vlen_element,
ESDM_def_enum,
ESDM_insert_enum,
ESDM_inq_enum_member,
ESDM_inq_enum_ident,
ESDM_def_opaque,
ESDM_def_var_deflate,
ESDM_def_var_fletcher32,
ESDM_def_var_chunking,
ESDM_def_var_endian,
ESDM_def_var_filter,
ESDM_set_var_chunk_cache,
ESDM_get_var_chunk_cache};

NC_Dispatch *esdm_dispatch_table = NULL;

int NC_ESDM_initialize(void) {
  int ret = NC_NOERR;
  esdm_status status;

  esdm_init();

  if (getenv("NC_ESDM_FORCEESDM_MKFS")) { // sole purpose is testing
    status = esdm_mkfs(ESDM_FORMAT_PURGE_RECREATE, ESDM_ACCESSIBILITY_GLOBAL);
    assert(status == ESDM_SUCCESS);
    status = esdm_mkfs(ESDM_FORMAT_PURGE_RECREATE, ESDM_ACCESSIBILITY_NODELOCAL);
    assert(status == ESDM_SUCCESS);
  }

  esdm_dispatch_table = &esdm_dispatcher;
  return ret;
}

int NC_ESDM_finalize(void) {
  esdm_finalize();
  return NC_NOERR;
}
