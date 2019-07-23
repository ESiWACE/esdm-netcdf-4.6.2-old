#include <stdlib.h>
//#include <libgen.h>
#include <assert.h>

#include <esdm.h>

#include "config.h"
#include "nc.h"
#include "ncdispatch.h"

#define NOT_IMPLEMENTED assert(0 && "NOT IMPLEMENTED");

#define ERROR(...) do{printf("[ESDM NC] ERROR %s:%d ",__func__, __LINE__); printf(__VA_ARGS__); exit(1); }while(0)

#define DEBUG(str) do{printf("[ESDM NC] DEBUG %s:%d %s\n",__func__, __LINE__, str);}while(0)
#define DEBUG_ENTER(...) do{printf("[ESDM NC] called %s:%d ",__func__, __LINE__); printf(__VA_ARGS__); }while(0)
#define WARN(...) do{printf("[ESDM NC] WARN %s:%d ",__func__, __LINE__); printf(__VA_ARGS__); }while(0)

#define WARN_NOT_IMPLEMENTED do{printf("[ESDM NC] WARN %s():%d NOT IMPLEMENTED\n",__func__, __LINE__); }while(0)
#define WARN_NOT_SUPPORTED do{printf("[ESDM NC] WARN %s():%d. NetCDF Feature not supported with ESDM!\n",__func__, __LINE__); }while(0)

typedef struct{
  int *dimidsp;
  esdm_dataset_t * dset;
} md_var_t;

typedef struct{
  int count;
  md_var_t ** var;
} md_vars_t;

typedef struct{
  int count;
  size_t * size;
  char ** name;
} nc_dim_tbl_t;

typedef struct{
  int ncid;
  int idp;
  esdm_container_t *c;
  // Some attributes provide information about the dataset as a whole and are called
  // global attributes. These are identified by the attribute name together with a blank
  // variable name (in CDL) or a special null "global variable" ID (in C or Fortran).
  nc_dim_tbl_t dimt;
  md_vars_t vars;
} nc_esdm_t;


static esdm_type_t type_nc_to_esdm(nc_type type){
  switch(type){
    case(NC_NAT): return SMD_DTYPE_UNKNOWN;
    case(NC_BYTE): return SMD_DTYPE_INT8;
    case(NC_CHAR): return SMD_DTYPE_CHAR;
    case(NC_SHORT): return SMD_DTYPE_INT16;
    case(NC_INT): return SMD_DTYPE_INT32;
    case(NC_FLOAT): return SMD_DTYPE_FLOAT;
    case(NC_DOUBLE): return SMD_DTYPE_DOUBLE;
    case(NC_UBYTE): return SMD_DTYPE_UINT8;
    case(NC_USHORT): return SMD_DTYPE_UINT16;
    case(NC_UINT): return SMD_DTYPE_UINT32;
    case(NC_INT64): return SMD_DTYPE_INT64;
    case(NC_UINT64): return SMD_DTYPE_UINT64;
    case(NC_STRING): return SMD_DTYPE_STRING;
    default:
      printf("ESDM does not support compound datatypes from NetCDF: %d\n", type);
      return NULL;
  }
}

static nc_type type_esdm_to_nc(esdm_type_t type){
  switch(type->type){
    case(SMD_TYPE_UNKNOWN): return NC_NAT;
    case(SMD_TYPE_INT8): return NC_BYTE;
    case(SMD_TYPE_CHAR): return NC_CHAR;
    case(SMD_TYPE_INT16): return NC_SHORT;
    case(SMD_TYPE_INT32): return NC_INT;
    case(SMD_TYPE_FLOAT): return NC_FLOAT;
    case(SMD_TYPE_DOUBLE): return NC_DOUBLE;
    case(SMD_TYPE_UINT8): return NC_UBYTE;
    case(SMD_TYPE_UINT16): return NC_USHORT;
    case(SMD_TYPE_UINT32): return NC_UINT;
    case(SMD_TYPE_INT64): return NC_INT64;
    case(SMD_TYPE_UINT64): return NC_UINT64;
    case(SMD_TYPE_STRING): return NC_STRING;
    default:
      printf("Unsupported datatype: %d\n", type->type);
      return 0;
  }
}

static inline nc_esdm_t * ESDM_nc_get_esdm_struct(int ncid){
  NC * ncp;
  if(NC_check_id(ncid, (NC**)&ncp) != NC_NOERR) return NULL;
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  if(e->ncid != ncid) return NULL;
  return e;
}

int lookup_md(md_vars_t * md, char * name, md_var_t ** value, int * pos){
  for(int i=0; i < md->count; i++){
    if(strcmp(name, esdm_dataset_name(md->var[i]->dset)) == 0){
      *value = md->var[i];
      *pos = i;
      return NC_NOERR;
    }
  }
  return NC_EBADID;
}

void insert_md(md_vars_t * md,  md_var_t * value){
  md->count++;
  md->var = realloc(md->var, md->count * sizeof(void*));
  if( ! md->var ){
    ERROR("Cannot allocate memory.");
  }
  md->var[md->count - 1] = value;
}

int ESDM_create(const char *path, int cmode, size_t initialsz, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  const char * realpath = path;

  if(strncmp(path, "esdm:", 5) == 0){
    realpath = & path[5];
  }else if(strncmp(path, "esd:", 4) == 0){
    realpath = & path[4];
  }
  //const char * base = basename(realpath);
  // remove leading slashes
  while(realpath[0] == '/'){
    realpath++;
  }
  char * cpath = strdup(realpath);
  // remove trailing slashes
  int pos = strlen(cpath) - 1;
  for( ; pos > 0; pos-- ){
    if (cpath[pos] != '/'){
      break;
    }
    cpath[pos] = '\0';
  }
  DEBUG_ENTER("%s %d %d %s\n", cpath, ncp->ext_ncid, ncp->int_ncid, ncp->path);

  nc_esdm_t * e = malloc(sizeof(nc_esdm_t));
  memset(e, 0, sizeof(nc_esdm_t));
  e->ncid = ncp->ext_ncid;

  int ret = esdm_container_create(cpath, 1, & e->c);
  free(cpath);
  if(ret != ESDM_SUCCESS){
    return NC_EBADID;
  }
  ncp->dispatchdata = e;

  return NC_NOERR;
}

static void  add_to_dims_tbl(nc_esdm_t * e, char const * name, size_t size){
  int cur = e->dimt.count;
  e->dimt.count++;
  int new = e->dimt.count;
  e->dimt.name = realloc(e->dimt.name, new * sizeof(void*));
  e->dimt.size = realloc(e->dimt.size, new * sizeof(size_t));

  if( ! e->dimt.name || ! e->dimt.size ){
    ERROR("Cannot allocate memory.");
  }

  e->dimt.name[cur] = strdup(name);
  e->dimt.size[cur] = size;
}

int ESDM_open(const char *path, int mode, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  const char * realpath = path;

  // We never use the variable mode!

  if(strncmp(path, "esdm:", 5) == 0){
    realpath = & path[5];
  }else if(strncmp(path, "esd:", 4) == 0){
    realpath = & path[4];
  }
  // remove leading slashes
  while(realpath[0] == '/'){
    realpath++;
  }
  char * cpath = strdup(realpath);
  // remove trailing slashes
  int pos = strlen(cpath) - 1;
  for( ; pos > 0; pos-- ){
    if (cpath[pos] != '/'){
      break;
    }
    cpath[pos] = '\0';
  }
  //const char * base = basename(realpath);

  DEBUG_ENTER("%s %d %d %s\n", cpath, ncp->ext_ncid, ncp->int_ncid, ncp->path);

  nc_esdm_t * e = malloc(sizeof(nc_esdm_t));
  memset(e, 0, sizeof(nc_esdm_t));
  e->ncid = ncp->ext_ncid;

  esdm_status ret;
  ret = esdm_container_open(cpath, 0, & e->c);
  free(cpath);

  if(ret != ESDM_SUCCESS) return NC_EBADID;

  ncp->dispatchdata = e;

  /*
   * Rebuild the dimension table
   * Algorithm:
   * for each dataset:
   *   open_dataset <= just leave it open
   *   inquire dataspace_named_dimensions()
   *   create a mapping from name to ID (ID == the new spot in the table); add the names => we do not know the dataset variable ID yet that implements it.
   * for each name in mapping:
   *    search the dataset number (new function) in the container and put it into the mapping
   *    Allow for same name with different dimensions if needed (can happen when compiling a container on the fly)
   */

  // now build the dim table
  esdm_container_t * c = e->c;
  int ndsets = esdm_container_dataset_count(c);
  // open all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++){
    esdm_dataset_t *dset = esdm_container_dataset_from_array (c, i);

    ret = esdm_dataset_ref(dset);
    if(ret != ESDM_SUCCESS){
      return NC_EINVAL;
    }

    esdm_dataspace_t * dspace;
    esdm_dataset_get_dataspace(dset, & dspace);
    int ndims = esdm_dataspace_get_dims(dspace);
    char const * const * names = NULL;
    ret = esdm_dataset_get_name_dims(dset, & names);
    if(ret != ESDM_SUCCESS){
      return NC_EINVAL;
    }

    md_var_t * evar = malloc(sizeof(md_var_t));
    evar->dimidsp = malloc(sizeof(int) * ndims);
    evar->dset = dset;
    int64_t const * dspace_size = esdm_dataspace_get_size(dspace);

    for (int j = 0; j < ndims; j++){
      // check if the dim already exists in the dim table
      int dim_found = -1;
      for(int k = 0; k < e->dimt.count; k++){
        if(strcmp(e->dimt.name[k], names[j]) == 0){
          // found it!
          if(e->dimt.size[k] != dspace_size[j]){
            WARN("Dimensions are not matching for %s", names[j]);
            return NC_EINVAL;
          }
          dim_found = k;
          break;
        }
      }
      if(dim_found == -1){
        dim_found = e->dimt.count;
        add_to_dims_tbl(e, names[j], dspace_size[j]);
      }
      evar->dimidsp[j] = dim_found;
    }
    insert_md(& e->vars, evar);
  }

  return NC_NOERR;
}

/**
 * @brief Put open netcdf dataset into define mode.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().

 * @return
 */

// I don't think we are dealing with modes yet.

int ESDM_redef(int ncid){
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align){

  int ret = NC_NOERR;
  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  ret = esdm_container_commit(e->c);
  if(ret != ESDM_SUCCESS){
    return NC_EBADID;
  }
  return NC_NOERR;
}

/**
 * @brief Synchronize an open netcdf dataset to disk. The function nc_sync() offers a way to synchronize the disk copy of a netCDF dataset with in-memory buffers.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @return
 */

 // I don't think we are dealing with this yet.

int ESDM_sync(int ncid){
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

/**
 * @brief No longer necessary for user to invoke manually. The function nc_abort() just closes the netCDF dataset, if not in define mode
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @return
 */

 // I don't think we are dealing with this yet.

int ESDM_abort(int ncid){
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_close(int ncid, void * b){

  int ret = NC_NOERR;
  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  esdm_container_commit(e->c);
  //esdm_container_destroy(e->c); // TODO disable for now, as we want to reread the file
  return NC_NOERR;
}

int ESDM_set_fill(int ncid, int fillmode, int *old_modep){
  DEBUG_ENTER("%d %d\n", ncid, fillmode);
  *old_modep = NC_NOFILL;

  // find the proper output for old_modep using fillmode
  // not clue why is returning segfault

  return NC_NOERR;
}

int ESDM_def_var_fill(int ncid, int varid, int no_fill, const void *fill_value){
  DEBUG_ENTER("%d %d\n", ncid, no_fill);
  return NC_NOERR;
}


int ESDM_var_par_access(int ncid, int varid, int access){
  DEBUG_ENTER("%d: var:%d access:%d\n", ncid, varid, access);
  return NC_NOERR;
}


int ESDM_inq_base_pe(int ncid, int *pe){ // for parallel execution
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_set_base_pe(int ncid, int pe){ // for parallel execution
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_format(int ncid, int *formatp){
  DEBUG_ENTER("%d\n", ncid);
  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if (!formatp)
    return NC_NOERR;

  *formatp = NC_FORMATX_ESDM;
  return NC_NOERR;
}

/**
 * @brief Obtain more detailed (vis-a-vis nc_inq_format) format information about an open dataset. Note that the netcdf API will present the file as if it had the format specified by nc_inq_format. The true file format, however, may not even be a netcdf file; it might be DAP, HDF4, or PNETCDF, for example. This function returns that true file type. It also returns the effective mode for the file.
 * @param ncid	NetCDF ID, from a previous call to nc_open() or nc_create().
 * @param formatp	Pointer to location for returned true format.
 * @param modep	Pointer to location for returned mode flags.
 * @return
 */

 // I don't think we are dealing with this yet.

int ESDM_inq_format_extended(int ncid, int *formatp, int* modep){
  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq(int ncid, int *ndimsp, int *nvarsp, int *nattsp, int *unlimdimidp){
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;
  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if(nattsp){
    smd_attr_t * attr;
    status = esdm_container_get_attributes(e->c, & attr);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
    *nattsp = attr->children;
  }
  if(ndimsp){
    *ndimsp = e->dimt.count;
  }
  if(nvarsp){
    int count = esdm_container_dataset_count(e->c);
    *nvarsp = count;
  }
  if(unlimdimidp){
    int ulimdim = -1;
    for(int i=0; e->dimt.count; i++){
      if(e->dimt.size[i] == 0){
        ulimdim = i;
        break;
      }
    }
    *unlimdimidp = ulimdim;
  }

  return NC_NOERR;
}

/**
 * @brief Inquire about a type. Given an ncid and a typeid, get the information about a type. This function will work on any type, including atomic and any user defined type, whether compound, opaque, enumeration, or variable length array.
 * @param ncid	The ncid for the group containing the type (ignored for atomic types).
 * @param xtype	The typeid for this type, as returned by nc_def_compound, nc_def_opaque, nc_def_enum, nc_def_vlen, or nc_inq_var, or as found in netcdf.h in the list of atomic types (NC_CHAR, NC_INT, etc.).
 * @param name	If non-NULL, the name of the user defined type will be copied here. It will be NC_MAX_NAME bytes or less. For atomic types, the type name from CDL will be given.
 * @param size	If non-NULL, the (in-memory) size of the type in bytes will be copied here. VLEN type size is the size of nc_vlen_t. String size is returned as the size of a character pointer. The size may be used to malloc space for the data, no matter what the type.
 * @return
 */

int ESDM_inq_type(int ncid, nc_type xtype, char *name, size_t *size){
  DEBUG_ENTER("%d\n", ncid);

// I'm not sure I can do it.

  return NC_NOERR;
}

int ESDM_def_dim(int ncid, const char *name, size_t len, int *idp){
  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  // ensure that the name hasn't been defined if it was defined, replace it
  for(int i=0; i < e->dimt.count; i++){
    if(strcmp(e->dimt.name[i], name) == 0){
      e->dimt.size[i] = len;
      return ret;
    }
  }

  int cnt = e->dimt.count;
  *idp = cnt;
  add_to_dims_tbl(e, name, len);
  DEBUG_ENTER("%d: %d\n", ncid, *idp);

  return NC_NOERR;
}

/**
 * @brief Find the ID of a dimension from the name.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
* @param name	Name of the dimension.
* @param idp	Pointer where dimension ID will be stored.
 * @return
 */

// Not tested yet ==> not working, see test tst_dim_ME

int ESDM_inq_dimid(int ncid, const char *name, int *idp){
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if (name == NULL) return NC_EACCESS;

  for(int i=0; i < e->dimt.count; i++){
    if(strcmp(e->dimt.name[i], name) == 0){
      *idp = i;
    }
  }

  return NC_NOERR;
}

int ESDM_inq_dim(int ncid, int dimid, char *name, size_t *lenp){
  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  assert(e->dimt.count > dimid);

  if(name != NULL){
    strcpy(name, e->dimt.name[dimid]);
  }
  if(lenp != NULL){
    *lenp = e->dimt.size[dimid];
  }

  return NC_NOERR;
}

/**
 * @brief Find the ID of the unlimited dimension.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param unlimdimidp	Pointer where unlimited dimension ID will be stored. If there is no unlimited dimension, -1 will be stored here. Ignored if NULL.
 * @return
 */

 // To be able to do this function, the unlimited dimensions have to be in the dimensions table. It may be already there, I have to check.

int ESDM_inq_unlimdim(int ncid, int *unlimdimidp){
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  return NC_NOERR;
}

// we may have sealed containers, that won't allow rename

/**
 * @brief Rename a dimension.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param dimid	Dimension ID, from a previous call to nc_inq_dimid() or nc_def_dim().
 * @param name	New name for dimension. Must be a null-terminated string with length less than NC_MAX_NAME.
 * @return
 */

 // It won't work if we have unlimited dimensions. It might work automatically if dimt.count is updated.

 // Not tested yet ==>> same problem as ESDM_inq_dimid

int ESDM_rename_dim(int ncid, int dimid, const char *name){
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if (name == NULL) return NC_EACCESS;

  for(int i=0; i < e->dimt.count; i++){
    if(strcmp(e->dimt.name[i], name) == 0){
      return NC_EACCESS; // Check if the name is taken
    }
  }

  e->dimt.name[dimid] = strdup(name);

  return NC_NOERR;
}

/**
 * @brief Return information about a netCDF attribute. The function nc_inq_att returns the attribute's type and length.
 * @paramncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @paramvarid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @paramname	Pointer to the location for the returned attribute NetCDF Names. Ignored if NULL.
 * @paramxtypep	Pointer to location for returned attribute Data Types. Ignored if NULL.
 * @paramlenp	Pointer to location for returned number of values currently stored in the attribute. For attributes of type NC_CHAR, you should not assume that this includes a trailing zero byte; it doesn't if the attribute was stored without a trailing zero byte, for example from a FORTRAN program. Before using the value as a C string, make sure it is null-terminated. Ignored if NULL.
 * @return
 */

// Tested and working

// missing lenp return

int ESDM_inq_att(int ncid, int varid, const char *name, nc_type *datatypep, size_t *lenp){
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;

  if (name == NULL) return NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t *attr;
  if(varid == NC_GLOBAL){
    status = esdm_container_get_attributes(e->c, & attr);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, & attr);
    if(status != ESDM_SUCCESS){
      return NC_EACCESS;
    }
  }

  smd_attr_t *a;
  a = smd_attr_get_child_by_name(attr, name);

  if(*datatypep){
    *datatypep = type_esdm_to_nc(a->type);
  }

  // if(*lenp){

// https://www.unidata.ucar.edu/software/netcdf/netcdf/Attributes.html
// The type and length of each attribute are not explicitly declared in CDL; they are derived from the values assigned to the attribute.

  // }

  return NC_NOERR;
}

int ESDM_inq_attid(int ncid, int varid, const char *name, int *attnump){
  DEBUG_ENTER("%d\n", ncid);
  esdm_status status;

  if (attnump == NULL) return NC_NOERR;

  if (name == NULL) return NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t * attr;
  if(varid == NC_GLOBAL){
    status = esdm_container_get_attributes(e->c, & attr);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, & attr);
    if(status != ESDM_SUCCESS){
      return NC_EACCESS;
    }
  }

  for (int i = 0; i < attr->children; i++){
    if (strcmp(name, attr->childs[i]->name) == 0){
      // *attnump = i;
      *attnump = attr->childs[i]->id;

      // The return for attnump is i or attr->childs[i]->id?!

      return NC_NOERR;
    }
  }

}

int ESDM_inq_attname(int ncid, int varid, int attnum, char *name){
  DEBUG_ENTER("%d\n", ncid);

  if (name == NULL) return NC_NOERR;

  assert(attnum >= 0);
  esdm_status status;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t * attr;
  if(varid == NC_GLOBAL){
    status = esdm_container_get_attributes(e->c, & attr);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, & attr);
    if(status != ESDM_SUCCESS){
      return NC_EACCESS;
    }
  }

  strcpy(name, attr->childs[attnum]->name);

  return NC_NOERR;
}

// we may have sealed containers, that won't allow rename

int ESDM_rename_att(int ncid, int varid, const char *name, const char *newname){
  DEBUG_ENTER("%d\n", ncid);
  esdm_status status;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t * attr;
  if(varid == NC_GLOBAL){
    status = esdm_container_get_attributes(e->c, & attr);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, & attr);
    if(status != ESDM_SUCCESS){
      return NC_EACCESS;
    }
  }

  int attnum;
  int ret = ESDM_inq_attid(ncid, varid, name, &attnum);
  free((void*)attr->childs[attnum]->name);
  attr->childs[attnum]->name = strdup(newname);

  return NC_NOERR;
}

/**
 * @brief Delete an attribute. The function nc_del_att() deletes a netCDF attribute from an open netCDF dataset. The netCDF dataset must be in define mode.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID of the attribute's variable, or NC_GLOBAL for a global attribute.
 * @param name	Attribute name.
 * @return NC_NOERR No error.
 * @return NC_EBADID Bad ncid.
 * @return NC_ENOTVAR Bad varid.
 * @return NC_EBADNAME Bad name.
 * @return NC_EINVAL Name not provided.
 * @return NC_EPERM File was opened read only.
 * @return NC_ENOTINDEFINE File is not in define mode.
 * @return NC_ENOTATT Attribute not found.
 * @return NC_EATTMETA Failure at HDF5 layer.
*/

int ESDM_del_att(int ncid, int varid, const char *name){

  esdm_status status;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t * attr;
  if(varid == NC_GLOBAL){
    status = esdm_container_delete_attribute(e->c, attr, name);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_delete_attribute(ev->dset, attr, name);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }

  DEBUG_ENTER("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_get_att(int ncid, int varid, const char* name, void* value, nc_type type){

  // if(type == NC_CHAR && strlen(value) > 0){
  //   etype = SMD_DTYPE_STRING;
  // }else{
  //   etype = type_nc_to_esdm(type);
  // }

  esdm_type_t etype;
  etype = type_nc_to_esdm(type);

  if(etype == NULL) {
    return NC_EINVAL;
  }
  if(name == NULL) {
    return NC_EACCESS;
  }
  esdm_status status;
  int ret;

// Conflict for git commit that I am not prepared to handle yet.
// -    size_t val = e->dimt.size[dimid];
// -    evar->dimidsp[i] = val;
// -    names[i] = e->dimt.name[dimid];
// -    bounds[i] = val;
// -    printf("%d = %ld\n", dimidsp[i], val);
// +    evar->dimidsp[i] = dimid;
// +    names[i]  = e->dimt.name[dimid];
// +    bounds[i] = e->dimt.size[dimid];
// +    // printf("%d = %ld\n", dimidsp[i], val);

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t * att;
  if(varid == NC_GLOBAL){
    status = esdm_container_get_attributes(e->c, & att);
    if(status != ESDM_SUCCESS) return NC_EACCESS;
  }else{
    if(varid > e->vars.count){
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    assert(ev != NULL);
    status = esdm_dataset_get_attributes(ev->dset, & att);
    if(status != ESDM_SUCCESS){
      smd_attr_destroy(att);
      return NC_EACCESS;
    }
  }

  smd_attr_t * child = smd_attr_get_child_by_name(att, name);
  if (child != NULL){
    smd_attr_copy_value(child, value);

    if(type == NC_CHAR && strlen(value) > 0){
      etype = SMD_DTYPE_STRING;
    }

    if(etype->type != child->type->type){
      return NC_EACCESS;
    }
    return NC_NOERR;
  }

  return NC_EACCESS;
}

int ESDM_put_att(int ncid, int varid, const char *name, nc_type datatype, size_t len, void const *value, nc_type type){
  esdm_type_t etype;

  if(type == NC_CHAR && len > 1){
    etype = SMD_DTYPE_STRING;
    len = 1;
  }else{
    etype = type_nc_to_esdm(datatype);
  }
  if(etype == NULL) {
    return NC_EACCESS;
  }

  int ret;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  // if(type != datatype && len > 0){
  //   // convert it to the right datatype on the fly, check if it exceeds the limits of the intended datatype
  //   // if it does fire NC_ERANGE
  //   // change value to new temporary value
  //   switch(datatype){
  //     case(NC_INT):{
  //         switch(type){
  //           case(NC_UINT):{
  //             int32_t *p = malloc(sizeof(int32_t) * len);
  //             uint32_t * o = (uint32_t*) value;
  //             value = p;
  //             for(int i=0; i < len; i++){
  //               p[i] = o[i];
  //               if(o[i] < 0 || o[i] > NC_MAX_INT){
  //                 free(p);
  //                 return NC_ERANGE;
  //               }
  //             }
  //           }
  //         }
  //     }
  //   }
  // }

  smd_attr_t * new;
  if(len > 1){
    smd_dtype_t * arr_type = smd_type_array(etype, len);
    new = smd_attr_new(name, arr_type, value, 0);
    //smd_type_unref(arr_type);
  }else{
    if(datatype == NC_STRING){
      new = smd_attr_new(name, etype, *(void**) value, 0);
    }else{
      new = smd_attr_new(name, etype, value, 0);
    }
  }

  if(type != datatype){
    free(value);
  }

  esdm_status status;

  if(varid == NC_GLOBAL){
    ret = esdm_container_link_attribute(e->c, 0, new);
  }else{
    if(varid > esdm_container_dataset_count(e->c)){
      smd_attr_destroy(new);
      return NC_EACCESS;
    }
    md_var_t * ev = e->vars.var[varid];
    ret = esdm_dataset_link_attribute(ev->dset, 0, new);
  }
  if(ret != ESDM_SUCCESS){
    smd_attr_destroy(new);
    return NC_EACCESS;
  }
  return NC_NOERR;
  // return NC_ESTRICTNC3;
}

int ESDM_def_var(int ncid, const char *name, nc_type xtype, int ndims, const int *dimidsp, int *varidp){

  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if (varidp){
    *varidp = e->vars.count;
    DEBUG_ENTER("%d: varid: %d\n", ncid, *varidp);
  }

  md_var_t * evar = malloc(sizeof(md_var_t));
  evar->dimidsp = malloc(sizeof(int) * ndims);

  int64_t bounds[ndims];
  char * names[ndims];
  for(int i=0; i < ndims; i++){
    int dimid = dimidsp[i];
    assert(e->dimt.count > dimid);

    size_t val = e->dimt.size[dimid];
    evar->dimidsp[i] = val;
    names[i] = e->dimt.name[dimid];
    bounds[i] = val;
    printf("%d = %ld\n", dimidsp[i], val);
  }

  esdm_type_t typ = type_nc_to_esdm(xtype);
  if(typ == SMD_DTYPE_UNKNOWN){
    return NC_EBADTYPE;
  }
  esdm_dataspace_t *dataspace;
  ret = esdm_dataspace_create(ndims, bounds, typ, &dataspace);

  esdm_dataset_t *d;
  ret = esdm_dataset_create(e->c, name, dataspace, & d);

  if(ret != ESDM_SUCCESS){
    return NC_EBADID;
  }
  ret = esdm_dataset_name_dims(d, names);

  evar->dset = d;
  insert_md(& e->vars, evar);

  return NC_NOERR;
}

/**
 * @brief Find the ID of a variable, from the name.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param name	Name of the variable.
 * @param varidp	Pointer to location for returned variable ID. Ignored if NULL.
 * @return
 */

 // Can we have global and local variables with the same name?

 // Not tested yet

int ESDM_inq_varid(int ncid, const char *name, int *varidp){
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;

  if (name == NULL) return NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t *attr;

// // find the id if the variable is global
//
//   status = esdm_container_get_attributes(e->c, & attr);
//   if(status != ESDM_SUCCESS) return NC_EACCESS;
//   esdm_container_t *c = e->c;
//
//   if (attr->children > 0){
//     smd_attr_t *a;
//     a = smd_attr_get_child_by_name(attr, name);
//     if (a != NULL){
//       *varidp = a->id; // should it be just -1?!
//       return NC_NOERR;
//     }
//   }
//
  // find the id if the variable is local

  esdm_container_t *c = e->c;
  int ndsets = esdm_container_dataset_count(c);
  // open all ESDM datasets, find the names
  for (int i = 0; i < ndsets; i++){
    esdm_dataset_t *dset = esdm_container_dataset_from_array (c, i);
    if (dset == NULL) return NC_EACCESS;

    char const *dname = esdm_dataset_name(dset);

    if (dname == NULL) return NC_NOERR;

    if (strcmp(dname, name) == 0){
      *varidp = i;
      return NC_NOERR;
    }
  }
  return NC_EACCESS;
}

// we may have sealed containers, that won't allow rename

/**
 * @brief Rename a variable.
 * @param ncid	NetCDF or group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), or associated inquiry functions such as nc_inq_ncid().
 * @param varid	Variable ID
 * @param name	New name of the variable.
 * @return
 */

 // Not tested yet

int ESDM_rename_var(int ncid, int varid, const char *name){
  DEBUG_ENTER("%d\n", ncid);

  esdm_status status;

  if (name == NULL) return NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  smd_attr_t *attr;
  if(varid > e->vars.count){
    return NC_EACCESS;
  }
  md_var_t * ev = e->vars.var[varid];
  assert(ev != NULL);

  // TODO test to avoid using the same name

  status = esdm_dataset_rename(ev->dset, name);
  if(status != ESDM_SUCCESS) return NC_EACCESS;

  return NC_NOERR;
}

int ESDM_get_vars(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type){
  DEBUG_ENTER("%d\n", ncid);

  int ret_NC = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  assert(e->vars.count > varid);
  md_var_t * kv = e->vars.var[varid];
  DEBUG_ENTER("%d type: %d buff: %p %p %p %p\n", ncid, mem_nc_type, data, startp, countp, stridep);

  // check the dimensions we actually want to write
  int access_all = 1;
  esdm_dataspace_t * space;
  int ret = esdm_dataset_get_dataspace(kv->dset, & space);

  if(mem_nc_type != type_esdm_to_nc(esdm_dataspace_get_type(space))){
    return NC_EBADTYPE;
  }
  int ndims = esdm_dataspace_get_dims(space);
  int64_t const* spacesize = esdm_dataspace_get_size(space);

  for(int i=0; i < ndims; i++){
    // printf(" - %zu %zu\n", startp[i], countp[i]);
    if(startp[i] != 0 || countp[i] != spacesize[i]){
      access_all = 0;
      break;
    }
  }
  if(access_all){
    esdm_status ret = esdm_read(kv->dset, (void *)data, space);
    if(ret != ESDM_SUCCESS){
      return NC_EINVAL;
    }
  }else{
    int64_t size[ndims];
    int64_t offset[ndims];
    for(int i=0; i < ndims; i++){
      size[i] = countp[i];
      offset[i] = startp[i];
    }
    esdm_dataspace_t *subspace;
    esdm_dataspace_subspace(space, ndims, size, offset, &subspace);
    esdm_status ret = esdm_read(kv->dset, (void *)data, subspace);
    if(ret != ESDM_SUCCESS){
      esdm_dataspace_destroy(subspace);
      return NC_EINVAL;
    }
    esdm_dataspace_destroy(subspace);
  }

  return NC_NOERR;
}

int ESDM_get_vara(int ncid, int varid, const size_t *startp, const size_t *countp, void *ip, int memtype)
{
  DEBUG_ENTER("%d\n", ncid);
  return ESDM_get_vars(ncid, varid, startp, countp, NULL, ip, memtype);
}

int ESDM_put_vars(int ncid, int varid, const size_t *startp, const size_t *countp, const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type){

  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  assert(e->vars.count > varid);
  md_var_t * kv = e->vars.var[varid];
  DEBUG_ENTER("%d type: %d buff: %p %p %p %p\n", ncid, mem_nc_type, data, startp, countp, stridep);

  // check the dimensions we actually want to write
  int access_all = 1;
  esdm_dataspace_t * space;
  ret = esdm_dataset_get_dataspace(kv->dset, & space);
  if(mem_nc_type != type_esdm_to_nc(esdm_dataspace_get_type(space))){
    return NC_EBADTYPE;
  }

  int ndims = esdm_dataspace_get_dims(space);
  int64_t const* spacesize = esdm_dataspace_get_size(space);

  for(int i=0; i < ndims; i++){
    // printf(" - %zu %zu\n", startp[i], countp[i]);
    if(startp[i] != 0 || countp[i] != spacesize[i]){
      access_all = 0;
      break;
    }
  }
  if(access_all){
    ret = esdm_write(kv->dset, (void *)data, space);
    if(ret != ESDM_SUCCESS){
      return NC_EINVAL;
    }
  }else{
    int64_t size[ndims];
    int64_t offset[ndims];
    for(int i=0; i < ndims; i++){
      size[i] = countp[i];
      offset[i] = startp[i];
    }
    esdm_dataspace_t *subspace;
    ret = esdm_dataspace_subspace(space, ndims, size, offset, &subspace);
    if(ret != ESDM_SUCCESS){
      return NC_EACCESS;
    }

    // assert(subspace->size);

    // 1097	        ESDM_LOG_FMT(ESDM_LOGLEVEL_DEBUG, "invalid arguments to `%s()` detected: `offset[%"PRId64"] + size[%"PRId64"] = %"PRId64" + %"PRId64" = %"PRId64"` is outside of the valid range for the dataspaces' dimension (offset %"PRId64", size %"PRId64")\n", __func__, i, i, offset[i], size[i], offset[i] + size[i], dataspace->offset[i], dataspace->size[i]);

    ret = esdm_write(kv->dset, (void *)data, subspace);
    if(ret != ESDM_SUCCESS){
      esdm_dataspace_destroy(subspace);
      return NC_EINVAL;
    }
    esdm_dataspace_destroy(subspace);
  }

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_put_vara(int ncid, int varid, const size_t *startp, const size_t *countp, const void *op, int memtype){
  DEBUG_ENTER("%d\n", ncid);
  return ESDM_put_vars(ncid, varid, startp, countp, NULL, op, memtype);
}

/**
* @brief Learn all about a variable.
* @param[in]	ncid	ncid for file.
* @param[in]	varid	varid for variable in question.
* @param[out]	name	Pointer to memory to contain the name of the variable.
* @param[out]	xtypep	Pointer to memory to contain the type of the variable.
* @param[out]	ndimsp	Pointer to memory to store the number of associated dimensions for the variable.
* @param[out]	dimidsp	Pointer to memory to store the dimids associated with the variable.
* @param[out]	nattsp	Pointer to memory to store the number of attributes associated with the variable.
* @param[out]	no_fill	Pointer to memory to store whether or not there is a fill value associated with the variable.
* @param[out]	fill_valuep	Pointer to memory to store the fill value (if one exists) for the variable.
* @param[out]	contiguousp	Pointer to memory to store contiguous-data information associated with the variable.
* @param[out]	endiannessp	Pointer to memory to store endianness value. One of NC_ENDIAN_BIG NC_ENDIAN_LITTLE NC_ENDIAN_NATIVE

// About compression, not supported.

* @param[out]	shufflep	Pointer to memory to store shuffle information associated with the variable.
* @param[out]	deflatep	Pointer to memory to store compression type associated with the variable.
* @param[out]	deflate_levelp	Pointer to memory to store compression level associated with the variable.
* @param[out]	fletcher32p	Pointer to memory to store compression information associated with the variable.
* @param[out]	chunksizesp	Pointer to memory to store chunksize information associated with the variable.
* @param[out]	idp	Pointer to memory to store filter id.
* @param[out]	nparamsp	Pointer to memory to store filter parameter count.
* @param[out]	params	Pointer to vector of unsigned integers into which to store filter parameters.
*/

// Not tested yet

int ESDM_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, int *ndimsp, int *dimidsp, int *nattsp, int *shufflep, int *deflatep, int *deflate_levelp, int *fletcher32p, int *contiguousp, size_t *chunksizesp, int *no_fill, void *fill_valuep, int *endiannessp, unsigned int* idp, size_t* nparamsp, unsigned int* params){

  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  DEBUG_ENTER("%d %d\n", ncid, varid);

  smd_attr_t * attr;

  if(varid == NC_GLOBAL){
    ret = esdm_container_link_attribute(e->c, 0, attr);

    if (name){
      name = strdup(e->dimt.name);
    }

    if (xtypep){
      *xtypep = type_esdm_to_nc(attr->type);
    }

    if (ndimsp){
      *ndimsp = e->dimt.size;
    }

    if (dimidsp){
      *dimidsp = e->dimt.count;
    }

    if (nattsp){
      *nattsp = attr->children;
    }

    // if (shufflep)
    //
    // if (deflatep)
    //
    // if (deflate_levelp)
    //
    // if (fletcher32p)
    //
    // if (contiguousp)
    //
    // if (chunksizesp)
    //
    // if (no_fill)
    //
    // if (fill_valuep)
    //
    // if (endiannessp)
    //
    // if (idp)
    //
    // if (nparamsp)
    //
    // if (params)

  }else{

      md_var_t * evar = e->vars.var[varid];
      assert(evar != NULL);

      esdm_dataspace_t * space;
      ret = esdm_dataset_get_dataspace(evar->dset, & space);

      if (name){
        strcpy(name, esdm_dataset_name(evar->dset));
        //name = strdup(esdm_dataset_name(evar->dset));
      }

      if(xtypep){
        *xtypep = type_esdm_to_nc(esdm_dataspace_get_type(space));
      }

      if(ndimsp){
        *ndimsp = esdm_dataspace_get_dims(space);
      }

      if(dimidsp){
        int ndims = esdm_dataspace_get_dims(space);
        for(int i=0; i < ndims; i++){
          dimidsp[i] = evar->dimidsp[i];
        }
      }

      if(nattsp){ // the number of attributes
        smd_attr_t * attr = NULL;
        ret = esdm_dataset_get_attributes(evar->dset, & attr);
        *nattsp = smd_attr_count(attr);
      }

      // if (shufflep)
      //
      // if (deflatep)
      //
      // if (deflate_levelp)
      //
      // if (fletcher32p)
      //
      // if (contiguousp)
      //
      // if (chunksizesp)
      //
      // if (no_fill)
      //
      // if (fill_valuep)
      //
      // if (endiannessp)
      //
      // if ( idp)
      //
      // if ( nparamsp)
      //
      // if ( params)

    }

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

 // Not working with groups yet.

static int ESDM_inq_typeids(int ncid, int *ntypes, int *p) {
  DEBUG_ENTER("%d\n", ncid);

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  if(ntypes){
    *ntypes = 0;
  }
  if(p){
    *p = 0;
  }

  return NC_NOERR; //check it later
}

/**
 * @brief
 * @param
 * @return
 */

static int ESDM_inq_typeid(int ncid, const char* name, nc_type* t)
{
  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  return NC_NOERR; //check it later
}

/**
 * @brief Print the metadata for a file.
 * @param ncid	The ncid of an open file.
 * @return
 */

// Printing the metadata for the container. Check it.

// Not tested yet

int ESDM_show_metadata(int ncid){
  DEBUG("");

  int ret = NC_NOERR;

  nc_esdm_t * e = ESDM_nc_get_esdm_struct(ncid);
  if(e == NULL) return NC_EBADID;

  // Should not call this function here...

  // smd_string_stream_t * s = smd_string_stream_create();
  // esdmI_container_metadata_create(e->c, s);
  //
  // if (s->string != NULL){
  //   printf("\nMetadata:\n\n%s", s->string);
  //   return NC_NOERR;
  // }

  return NC_EACCESS;
}

/**
 * @brief Return number and list of unlimited dimensions.
 * @param ncid	NetCDF file and group ID, from a previous call to nc_open(), nc_create(), nc_def_grp(), etc.
 * @param nunlimdimsp	A pointer to an int which will get the number of visible unlimited dimensions. Ignored if NULL.
 * @param unlimdimidsp	A pointer to an already allocated array of int which will get the ids of all visible unlimited dimensions. Ignored if NULL. To allocate the correct length for this array, call nc_inq_unlimdims with a NULL for this parameter and use the nunlimdimsp parameter to get the number of visible unlimited dimensions.
 * @return
 */

int ESDM_inq_unlimdims(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

// Same idea of ESDM_inq_unlimdim, but returning a list

  return NC_NOERR;
}

/**
 * @brief Return the group ID for a group given the name.
* @param_in	ncid	A valid file or group ncid.
* @param_in	name	The name of the group you are querying.
* @param_out	grp_ncid	Pointer to memory to hold the group ncid.
 * @return
 */

int ESDM_inq_ncid(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

// Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_grps(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_grpname(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_grpname_full(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_grp_parent(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_grp_full_ncid(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief Get a list of varids associated with a group given a group ID.
 * @param
 * @return
 */

int ESDM_inq_varids(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief Retrieve a list of dimension ids associated with a group.
 * @param
 * @return
 */

int ESDM_inq_dimids(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief Learn if two types are equal.
 * @param ncid1	NetCDF ID of first typeid.
 * @param typeid1	First typeid.
 * @param ncid2	NetCDF ID of second typeid.
 * @param typeid2	Second typeid.
 * @param equal	Pointer to int. A non-zero value will be copied here if the two types are equal, a zero if they are not equal.
 * @return
 */

 // Not tested yet

int ESDM_inq_type_equal(int ncid1, nc_type typeid1, int ncid2, nc_type typeid2, int *equal){
  DEBUG("");

  nc_esdm_t * e1 = ESDM_nc_get_esdm_struct(ncid1);
  if(e1 == NULL) return NC_EBADID;

  nc_esdm_t * e2 = ESDM_nc_get_esdm_struct(ncid2);
  if(e2 == NULL) return NC_EBADID;

  // It must probably not be this

  if (typeid1 == typeid2)
    *equal = 1;
  else *equal = 0;

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_grp(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_rename_grp(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // Function using groups

  return NC_NOERR;
}

/**
 * @brief Learn about a user defined type.
 * @param ncid	NetCDF ID
 * @param xtype	The typeid
 * @param name	The NetCDF Names will be copied here. Ignored if NULL.
 * @param size	the (in-memory) size of the type in bytes will be copied here. VLEN type size is the size of nc_vlen_t. String size is returned as the size of a character pointer. The size may be used to malloc space for the data, no matter what the type. Ignored if NULL.
 * @param base_nc_typep	The base type will be copied here for enum and VLEN types. Ignored if NULL.
 * @param nfieldsp	The number of fields will be copied here for enum and compound types. Ignored if NULL.
 * @param classp	Return the class of the user defined type, NC_VLEN, NC_OPAQUE, NC_ENUM, or NC_COMPOUND. Ignored if NULL.
 * @return
 */

int ESDM_inq_user_type(int ncid, nc_type xtype, char *name, size_t *size, nc_type *base_nc_typep, size_t *nfieldsp, int *classp){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

// I'm not sure what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_compound(){
  DEBUG("");
  WARN_NOT_SUPPORTED;
  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_insert_compound(){
  DEBUG("");
  WARN_NOT_SUPPORTED;
  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_insert_array_compound(){
  DEBUG("");
  WARN_NOT_SUPPORTED;
  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_compound_field(){
  DEBUG("");
  WARN_NOT_SUPPORTED;
  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_compound_fieldindex(){
  DEBUG("");
  WARN_NOT_SUPPORTED;
  return NC_NOERR;
}

/**
 * @brief Use this function to define a variable length array type.
 * @param
 * @return
 */

int ESDM_def_vlen(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't think ESDM support this like that.

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_put_vlen_element(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't think ESDM support this like that.

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_get_vlen_element(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't think ESDM support this like that.

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_enum(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_insert_enum(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_enum_member(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_inq_enum_ident(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_opaque(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_NOERR;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_var_deflate(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_var_fletcher32(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_var_chunking(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_var_endian(){
  DEBUG("");
  WARN_NOT_IMPLEMENTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_def_var_filter(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_set_var_chunk_cache(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

/**
 * @brief
 * @param
 * @return
 */

int ESDM_get_var_chunk_cache(){
  DEBUG("");
  WARN_NOT_SUPPORTED;

  // I don't know what to do

  return NC_EINVAL;
}

static NC_Dispatch esdm_dispatcher = {
  NC_FORMATX_ESDM,

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
  ESDM_get_var_chunk_cache
};

NC_Dispatch* esdm_dispatch_table = NULL;

int NC_ESDM_initialize(void) {
   int ret = NC_NOERR;
   esdm_status status;

   esdm_init();

   status = esdm_mkfs(ESDM_FORMAT_PURGE_RECREATE, ESDM_ACCESSIBILITY_GLOBAL);
   assert(status == ESDM_SUCCESS);
   status = esdm_mkfs(ESDM_FORMAT_PURGE_RECREATE, ESDM_ACCESSIBILITY_NODELOCAL);
   assert(status == ESDM_SUCCESS);

   esdm_dispatch_table = &esdm_dispatcher;
   return ret;
}

int NC_ESDM_finalize(void){
  esdm_finalize();
  return NC_NOERR;
}
