#include <stdlib.h>

#include "config.h"
#include "nc.h"
#include "ncdispatch.h"

#define debug(...) do{printf("called %s: %d",__func__, __LINE__); printf(__VA_ARGS__); }while(0)


void ESDM_create(){
  debug("");
}
void ESDM_open(){
  debug("");
}
void ESDM_redef(){
  debug("");
}
void ESDM__enddef(){
  debug("");
}
void ESDM_sync(){
  debug("");
}
void ESDM_abort(){
  debug("");
}
void ESDM_close(){
  debug("");
}
void ESDM_set_fill(){
  debug("");
}
void ESDM_inq_format(){
  debug("");
}
void ESDM_inq_format_extended(){
  debug("");
}
void ESDM_inq(){
  debug("");
}
void ESDM_inq_type(){
  debug("");
}
void ESDM_def_dim(){
  debug("");
}
void ESDM_inq_dimid(){
  debug("");
}
void ESDM_inq_dim(){
  debug("");
}
void ESDM_inq_unlimdim(){
  debug("");
}
void ESDM_rename_dim(){
  debug("");
}
void ESDM_inq_att(){
  debug("");
}
void ESDM_inq_attid(){
  debug("");
}
void ESDM_inq_attname(){
  debug("");
}
void ESDM_rename_att(){
  debug("");
}
void ESDM_del_att(){
  debug("");
}
void ESDM_get_att(){
  debug("");
}
void ESDM_put_att(){
  debug("");
}
void ESDM_def_var(){
  debug("");
}
void ESDM_inq_varid(){
  debug("");
}
void ESDM_rename_var(){
  debug("");
}
void ESDM_get_vara(){
  debug("");
}
void ESDM_put_vara(){
  debug("");
}
void ESDM_get_vars(){
  debug("");
}
void ESDM_put_vars(){
  debug("");
}
void ESDM_inq_var_all(){
  debug("");
}
void ESDM_var_par_access(){
  debug("");
}
void ESDM_def_var_fill(){
  debug("");
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

   esdm_dispatch_table = &esdm_dispatcher;
   return ret;
}

int NC_ESDM_finalize(void){
  return NC_NOERR;
}
