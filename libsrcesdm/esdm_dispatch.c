#include <stdlib.h>

#include "config.h"
#include "nc.h"
#include "ncdispatch.h"

#define debug(...) do{printf("called %s: %d",__func__, __LINE__); printf(__VA_ARGS__); }while(0)


int ESDM_create(const char *path, int cmode, size_t initialsz, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  debug("");
  return NC_NOERR;
}

int ESDM_open(const char *path, int mode, int basepe, size_t *chunksizehintp, void* parameters, struct NC_Dispatch* table, NC* ncp){
  debug("");
  return NC_NOERR;
}

int ESDM_redef(int ncid){
  debug("");
  return NC_NOERR;
}

int ESDM__enddef(int ncid, size_t h_minfree, size_t v_align,
	size_t v_minfree, size_t r_align){
  debug("");
  return NC_NOERR;
}

int ESDM_sync(int ncid){
  debug("");
  return NC_NOERR;
}

int ESDM_abort(int ncid){
  debug("");
  return NC_NOERR;
}

int ESDM_close(int ncid,void* b){
  debug("");
  return NC_NOERR;
}

int ESDM_set_fill(int ncid, int fillmode, int *old_modep){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_base_pe(int ncid, int *pe){
  debug("");
  return NC_NOERR;
}

int ESDM_set_base_pe(int ncid, int pe){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_format(int ncid, int *formatp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_format_extended(int ncid, int *formatp, int* modep){
  debug("");
  return NC_NOERR;
}

int ESDM_inq(int ncid, int *ndimsp, int *nvarsp, int *nattsp, int *unlimdimidp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_type(int ncid, nc_type xtype, char *name, size_t *size){
  debug("");
  return NC_NOERR;
}

int ESDM_def_dim(int ncid, const char *name, size_t len, int *idp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_dimid(int ncid, const char *name, int *idp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_dim(int ncid, int dimid, char *name, size_t *lenp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_unlimdim(int ncid, int *unlimdimidp){
  debug("");
  return NC_NOERR;
}

int ESDM_rename_dim(int ncid, int dimid, const char *name){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_att(int ncid, int varid, const char *name,  nc_type *datatypep, size_t *lenp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_attid(int ncid, int varid, const char *name, int *attnump){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_attname(int ncid, int varid, int attnum, char *name){
  debug("");
  return NC_NOERR;
}

int ESDM_rename_att(int ncid, int varid, const char *name, const char *newname){
  debug("");
  return NC_NOERR;
}

int ESDM_del_att(int ncid, int varid, const char *name){
  debug("");
  return NC_NOERR;
}

int ESDM_get_att(int ncid, int varid, const char* name, void* value, nc_type t){
  debug("");
  return NC_NOERR;
}

int ESDM_put_att(int ncid, int varid, const char *name, nc_type xtype,
           size_t len, const void *op){
  debug("");
  return NC_NOERR;
}

int ESDM_def_var(int ncid, const char *name, nc_type xtype,
            int ndims, const int *dimidsp, int *varidp){
  debug("");
  return NC_NOERR;
}

int ESDM_inq_varid(int ncid, const char *name, int *varidp){
  debug("");
  return NC_NOERR;
}

int ESDM_rename_var(int ncid, int varid, const char *name){
  debug("");
  return NC_NOERR;
}


int ESDM_get_vars(int ncid, int varid, const size_t *startp, const size_t *countp,
             const ptrdiff_t *stridep, void *data, nc_type mem_nc_type){
  debug("");
  return NC_NOERR;
}

int ESDM_get_vara(int ncid, int varid, const size_t *startp,
             const size_t *countp, void *ip, int memtype)
{
  debug("");
  return ESDM_get_vars(ncid, varid, startp, countp, NULL, ip, memtype);
}

int ESDM_put_vars(int ncid, int varid, const size_t *startp, const size_t *countp,
             const ptrdiff_t *stridep, const void *data, nc_type mem_nc_type){
  debug("");
  return NC_NOERR;
}

int ESDM_put_vara(int ncid, int varid, const size_t *startp,
             const size_t *countp, const void *op, int memtype)
{
  debug("");
  return ESDM_put_vars(ncid, varid, startp, countp, NULL, op, memtype);
}

int ESDM_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, int *ndimsp, int *dimidsp, int *nattsp, int *shufflep, int *deflatep, int *deflate_levelp, int *fletcher32p, int *contiguousp, size_t *chunksizesp, int *no_fill, void *fill_valuep, int *endiannessp, unsigned int* idp, size_t* nparamsp, unsigned int* params ){
  debug("");
  return NC_NOERR;
}


// int ESDM_var_par_access(int, int, int){
//  debug("");
//  return NC_NOERR;
//}

// int ESDM_def_var_fill(int, int, int, const void*){
//  debug("");
//  return NC_NOERR;
//}



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

  ESDM_inq_var_all//,
  //ESDM_var_par_access,
  //ESDM_def_var_fill
};

NC_Dispatch* esdm_dispatch_table = NULL;

int NC_ESDM_initialize(void) {
   int ret = NC_NOERR;

   esdm_dispatch_table = &esdm_dispatcher;
   return ret;
}

int NC_ESDM_finalize(void){
  return NC_NOERR;
}
