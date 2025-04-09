#include "ds/_gc_vector.h"
#include "_gc_shared.h"
#include "ds/_gc_array.h"

void __gc_vec_init(struct __GCVector* vec, size_t cap,
        size_t el_size, gc_status* out_status)
{
    gc_status _status;

    __gc_arr_init((struct __GCArray*)vec, cap, el_size, &_status);

    GC_VRETURN(out_status, _status);
}

/* Destroys the provided struct __GCVector. */
void __gc_vec_destroy(struct __GCVector* vec)
{
    __gc_arr_destroy((struct __GCArray*)vec);
}
