#ifndef __GC_SHARED_H__
#define __GC_SHARED_H__

#define GC_RETURN(ret_val, out_err_p, out_err_v)                               \
    if(out_err_p != NULL) *out_err_p = out_err_v;                              \
    return ret_val                                                             \

#define GC_VRETURN(out_err_p, out_err_v)                                       \
    if(out_err_p != NULL) *out_err_p = out_err_v;                              \
    return                                                                     \

#endif // __GC_SHARED_H__
