#include <stdlib.h>
//#include <libgen.h>
#include <assert.h>

#include <esdm.h>

#include "config.h"
#include "nc.h"
#include "ncdispatch.h"

#define NOT_IMPLEMENTED assert(0 && "NOT IMPLEMENTED");

#define debug(...) do{printf("called %s: %d ",__func__, __LINE__); printf(__VA_ARGS__); }while(0)

typedef struct{
  nc_type type;
  int ndims;
  int *dimidsp;
  esdm_dataspace_t * space;
  esdm_dataset_t * dset;
} md_entity_var_t;

typedef struct{
  char * name;
  void * value;
} md_entity_t;

// This structure contains a flat mday for the metadata name
// it is used up to a specific size of the metadata table before
// the hashmap implementation is beneficial
typedef struct{
  int size;
  md_entity_t kv[10];
} metadata_t;

typedef struct{
  esdm_container * c;
  metadata_t attr;
  metadata_t dims;
  metadata_t vars;
} nc_esdm_t;

static esdm_datatype type_nc_to_esdm(nc_type type){
  switch(type){
    case(NC_NAT): return ESDM_TYPE_UNKNOWN;
    case(NC_BYTE): return ESDM_TYPE_INT8_T;
    case(NC_CHAR): return ESDM_TYPE_CHAR;
    case(NC_SHORT): return ESDM_TYPE_INT16_T;
    case(NC_INT): return ESDM_TYPE_INT32_T;
    case(NC_FLOAT): return ESDM_TYPE_FLOAT;
    case(NC_DOUBLE): return ESDM_TYPE_DOUBLE;
    case(NC_UBYTE): return ESDM_TYPE_UINT8_T;
    case(NC_USHORT): return ESDM_TYPE_UINT16_T;
    case(NC_UINT): return ESDM_TYPE_UINT32_T;
    case(NC_INT64): return ESDM_TYPE_INT64_T;
    case(NC_UINT64): return ESDM_TYPE_UINT64_T;
    case(NC_STRING): return ESDM_TYPE_STRING;
    default:
      printf("Unsupported datatype: %d\n", type);
      return 0;
  }
}

static nc_type type_esdm_to_nc(esdm_datatype type){
  switch(type){
    case(ESDM_TYPE_UNKNOWN): return NC_NAT;
    case(ESDM_TYPE_INT8_T): return NC_BYTE;
    case(ESDM_TYPE_CHAR): return NC_CHAR;
    case(ESDM_TYPE_INT16_T): return NC_SHORT;
    case(ESDM_TYPE_INT32_T): return NC_INT;
    case(ESDM_TYPE_FLOAT): return NC_FLOAT;
    case(ESDM_TYPE_DOUBLE): return NC_DOUBLE;
    case(ESDM_TYPE_UINT8_T): return NC_UBYTE;
    case(ESDM_TYPE_UINT16_T): return NC_USHORT;
    case(ESDM_TYPE_UINT32_T): return NC_UINT;
    case(ESDM_TYPE_INT64_T): return NC_INT64;
    case(ESDM_TYPE_UINT64_T): return NC_UINT64;
    case(ESDM_TYPE_STRING): return NC_STRING;
    default:
      printf("Unsupported datatype: %d\n", type);
      return 0;
  }
}


int lookup_md(metadata_t * md, char * name, void ** value, int * pos){
  for(int i=0; i < md->size; i++){
    if(strcmp(name, md->kv[i].name) == 0){
      *value = & md->kv[i].value;
      *pos = i;
      return NC_NOERR;
    }
  }
  return NC_EBADID;
}

int insert_md(metadata_t * md, const char * name, void * value, int * pos){
  assert(md->size < 10);
  md->kv[md->size].name = strdup(name);
  md->kv[md->size].value = value;
  *pos = md->size;
  md->size++;
  return NC_NOERR;
}


int ESDM_create(const char *path, int cmode, size_t initialsz, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  const char * realpath = path;

  if(strncmp(path, "esdm:", 5) == 0){
    realpath = & path[5];
  }else if(strncmp(path, "esd:", 4) == 0){
    realpath = & path[4];
  }
  //const char * base = basename(realpath);

  debug("%s %d %d %s\n", realpath, ncp->ext_ncid, ncp->int_ncid, ncp->path);

  nc_esdm_t * e = malloc(sizeof(nc_esdm_t));
  memset(e, 0, sizeof(nc_esdm_t));
  e->c = esdm_container_create(realpath);
  ncp->dispatchdata = e;

  return NC_NOERR;
}

int ESDM_open(const char *path, int mode, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  int ncid = 0;
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_redef(int ncid){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM__enddef(int ncid, size_t h_minfree, size_t v_align, size_t v_minfree, size_t r_align){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d\n", ncid);

  esdm_container_commit(e->c);
  return NC_NOERR;
}

int ESDM_sync(int ncid){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_abort(int ncid){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_close(int ncid, void * b){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d\n", ncid);
  esdm_container_destroy(e->c);
  return NC_NOERR;
}

int ESDM_set_fill(int ncid, int fillmode, int *old_modep){
  debug("%d %d\n", ncid, fillmode);
  *old_modep = NC_NOFILL;
  return NC_NOERR;
}

int ESDM_def_var_fill(int ncid, int varid, int no_fill, const void *fill_value){
  debug("%d %d\n", ncid, no_fill);
  return NC_NOERR;
}


int ESDM_var_par_access(int ncid, int varid, int access){
  debug("%d: var:%d access:%d\n", ncid, varid, access);
  return NC_NOERR;
}


int ESDM_inq_base_pe(int ncid, int *pe){ // for parallel execution
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_set_base_pe(int ncid, int pe){ // for parallel execution
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_format(int ncid, int *formatp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_format_extended(int ncid, int *formatp, int* modep){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq(int ncid, int *ndimsp, int *nvarsp, int *nattsp, int *unlimdimidp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_type(int ncid, nc_type xtype, char *name, size_t *size){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_def_dim(int ncid, const char *name, size_t len, int *idp){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d\n", ncid);
  ret = insert_md(& e->dims, name, (size_t*) len, idp);

  return ret;
}

int ESDM_inq_dimid(int ncid, const char *name, int *idp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_dim(int ncid, int dimid, char *name, size_t *lenp){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d %d %s\n", ncid, dimid, name);

  assert(e->dims.size > dimid);

  md_entity_t * kv = & e->dims.kv[dimid];
  if(name != NULL){
    strcpy(name, kv->name);
  }
  if(lenp != NULL){
    *lenp = (size_t) kv->value;
  }

  return NC_NOERR;
}

int ESDM_inq_unlimdim(int ncid, int *unlimdimidp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_rename_dim(int ncid, int dimid, const char *name){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_att(int ncid, int varid, const char *name, nc_type *datatypep, size_t *lenp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_attid(int ncid, int varid, const char *name, int *attnump){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_inq_attname(int ncid, int varid, int attnum, char *name){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_rename_att(int ncid, int varid, const char *name, const char *newname){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_del_att(int ncid, int varid, const char *name){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_get_att(int ncid, int varid, const char* name, void* value, nc_type t){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_put_att(int ncid, int varid, const char *name, nc_type datatype,
	   size_t len, const void *value, nc_type type){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_def_var(int ncid, const char *name, nc_type xtype,
            int ndims, const int *dimidsp, int *varidp){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d\n", ncid);

  md_entity_var_t * evar = malloc(sizeof(md_entity_var_t));
  evar->type = xtype;
  evar->ndims = ndims;
  evar->dimidsp = malloc(sizeof(int) * ndims);

  int64_t bounds[ndims];
  for(int i=0; i < ndims; i++){
    int dimid = dimidsp[i];
    assert(e->dims.size > dimid);
    md_entity_t * md = & e->dims.kv[dimid];
    size_t val = (size_t) md->value;
    evar->dimidsp[i] = dimid;
    printf("%d %s %zd\n", dimidsp[i], md->name, val);
    bounds[i] = val;
  }

  esdm_datatype typ = type_nc_to_esdm(xtype);
  if(typ == ESDM_TYPE_UNKNOWN){
    return NC_EBADTYPE;
  }
  esdm_dataspace_t * dataspace = esdm_dataspace_create(ndims, bounds, typ);
  esdm_dataset_t * dataset = esdm_dataset_create(e->c, name, dataspace);
  if(dataset == NULL){
    return NC_EBADID;
  }
  evar->dset = dataset;
  evar->space = dataspace;
  insert_md(& e->vars, name, evar, varidp);

  return NC_NOERR;
}

int ESDM_inq_varid(int ncid, const char *name, int *varidp){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_rename_var(int ncid, int varid, const char *name){
  debug("%d\n", ncid);
  return NC_NOERR;
}


int ESDM_get_vars(int ncid, int varid, const size_t *startp, const size_t *countp,
             const ptrdiff_t *stridep, void *data, nc_type mem_nc_type){
  debug("%d\n", ncid);
  return NC_NOERR;
}

int ESDM_get_vara(int ncid, int varid, const size_t *startp,
             const size_t *countp, void *ip, int memtype)
{
  debug("%d\n", ncid);
  return ESDM_get_vars(ncid, varid, startp, countp, NULL, ip, memtype);
}

int ESDM_put_vars(int ncid, int varid, const size_t *startp, const size_t *countp,
             const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  assert(e->vars.size > varid);
  md_entity_var_t * kv = e->vars.kv[varid].value;
  debug("%d type: %d buff: %p %p %p %p\n", ncid, mem_nc_type, data, startp, countp, stridep);
  if(mem_nc_type != kv->type){
    return NC_EBADTYPE;
  }
  // check the dimensions we actually want to write
  int access_all = 1;
  esdm_dataspace_t * space = kv->space;
  for(int i=0; i < kv->ndims; i++){
    printf(" - %zu %zu\n", startp[i], countp[i]);
    if(startp[i] != 0 || countp[i] != space->size[i]){
      access_all = 0;
      break;
    }
  }
  if(access_all){
    ret = esdm_write(kv->dset, data, space);
    if(ret != ESDM_SUCCESS){
      return NC_EINVAL;
    }
  }else{
    int64_t size[kv->ndims];
    int64_t offset[kv->ndims];
    for(int i=0; i < kv->ndims; i++){
      size[i] = countp[i];
      offset[i] = startp[i];
    }
    esdm_dataspace_t * subspace = esdm_dataspace_subspace(space, kv->ndims, size, offset);
    ret = esdm_write(kv->dset, data, subspace);
    if(ret != ESDM_SUCCESS){
      esdm_dataspace_destroy(subspace);
      return NC_EINVAL;
    }
    esdm_dataspace_destroy(subspace);
  }

  return NC_NOERR;
}

int ESDM_put_vara(int ncid, int varid, const size_t *startp,
             const size_t *countp, const void *op, int memtype)
{
  debug("%d\n", ncid);
  return ESDM_put_vars(ncid, varid, startp, countp, NULL, op, memtype);
}

int ESDM_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, int *ndimsp, int *dimidsp, int *nattsp, int *shufflep, int *deflatep, int *deflate_levelp, int *fletcher32p, int *contiguousp, size_t *chunksizesp, int *no_fill, void *fill_valuep, int *endiannessp, unsigned int* idp, size_t* nparamsp, unsigned int* params ){
  NC * ncp;
  int ret = NC_NOERR;
  if((ret = NC_check_id(ncid, (NC**)&ncp)) != NC_NOERR) return (ret);
  nc_esdm_t * e = (nc_esdm_t *) ncp->dispatchdata;
  debug("%d %d\n", ncid, varid);
  assert(e->vars.size > varid);

  md_entity_t * kv = & e->vars.kv[varid];
  md_entity_var_t * evar = (md_entity_var_t *) kv->value;
  if(name != NULL){
    strcpy(name, kv->name);
  }
  if(xtypep){
    *xtypep = evar->type;
  }
  if(ndimsp){
    *ndimsp = evar->ndims;
  }
  if(dimidsp){
    for(int i=0; i < evar->ndims; i++){
      dimidsp[i] = evar->dimidsp[i];
    }
  }
  // esdm_dataset_t * dataset
  return NC_NOERR;
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
  ESDM_def_var_fill
};

NC_Dispatch* esdm_dispatch_table = NULL;

int NC_ESDM_initialize(void) {
   int ret = NC_NOERR;

   esdm_init();
   esdm_dispatch_table = &esdm_dispatcher;
   return ret;
}

int NC_ESDM_finalize(void){
  esdm_finalize();
  return NC_NOERR;
}
