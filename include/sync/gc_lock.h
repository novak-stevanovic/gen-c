#ifndef _GC_LOCK_H_
#define _GC_LOCK_H_

#include "gc_shared.h"

/* GCLock is a simple, thread-safe synchronization mechanism that functions
 * similarly to a mutex, although it offers fewer features.
 *
 * GCLock is designed to be used inside event handlers(it is async-safe).
 * It is based on POSIX-compliant pipe() and poll() making it
 * portable. It is suitable for both process and thread synchronization.
 *
 * Using this lock on a thread that has 'SA_RESTART' flag enabled is currently
 * undefined behavior. */

typedef struct GCLock
{
    int _pipefd[2];
} GCLock;

/* -------------------------------------------------------------------------- */

/* Call this function during your application's initialization to set up the
* lock. It prepares the underlying resources so that the lock can be used for
* synchronization.
*
* ERROR CODES:
* On success: GC_SUCCESS,
* On failure: One of the error codes indicating failure:
*   1. GC_ERR_LOCK_CAP_REACHED - The per-process or system-wide limit on the
* number of open file descriptors has been reached. It may also indicate
* that the user hard limit on memory allocated for pipes has been
* reached.
*   2. GC_ERR_UNHANDLED - An unhandled error has happened. Continuing to use
* the same lock results in undefined behavior. Under proper usage, this
* error is not expected to occur. */

void gc_lock_init(GCLock* lock, gc_status* out_err);

/* -------------------------------------------------------------------------- */

/* Use this function to clean up and free the lock's resources (such as file
* descriptors) when it is no longer needed. This ensures that no resources
* are leaked.
*
* ERROR CODES:
* On success: GC_SUCCESS,
* On failure: One of the error codes indicating failure:
*   1. GC_ERR_UNHANDLED - An unhandled error has happened. Continuing to use
* the same lock results in undefined behavior. Under proper usage, this
* error is not expected to occur. */

void gc_lock_destroy(GCLock* lock, gc_status* out_err);

/* -------------------------------------------------------------------------- */

/* Use this function to block the current thread (or process) until the lock
* is signaled. If the lock is already in a signaled state when wait() is 
* called, the function returns immediately, allowing the thread to proceed.
* Otherwise, it blocks until a corresponding signal is received.
*
* ERROR CODES:
* On success: GC_SUCCESS,
* On failure: One of the error codes indicating failure:
*   1. GC_ERR_UNHANDLED - An unhandled error has happened. Continuing to use
* the same lock results in undefined behavior. Under proper usage, this
* error is not expected to occur. */

void gc_lock_wait(GCLock* lock, gc_status* out_err);

/* -------------------------------------------------------------------------- */

/* Call this function to signal (or release) the lock, waking up any thread
* waiting in gc_lock_wait(). Calling signal() twice(or more) in succession,
* results in the extra signals being ignored; the lock remains in a signaled
* state without queuing additional signals. This means that if wait() is called
* afterward, it will see the lock as already signaled and return immediately.
*
* ERROR CODES:
* On success: GC_SUCCESS,
* On failure: One of the error codes indicating failure:
*   1. GC_ERR_UNHANDLED - An unhandled error has happened. Continuing to use
* the same lock results in undefined behavior. Under proper usage, this
* error is not expected to occur. */

void gc_lock_signal(GCLock* lock, gc_status* out_err);

/* -------------------------------------------------------------------------- */

#endif // _GC_LOCK_H_
