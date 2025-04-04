#include "sync/gc_lock.h"
#include "_gc_shared.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

void gc_lock_init(GCLock* lock, gc_status* out_err)
{
    int pipe_status = pipe(lock->_pipefd);

    if(pipe_status != 0)
    {
        if((errno == EMFILE) || (errno == ENFILE))
        {
            GC_VRETURN(out_err, GC_ERR_LOCK_CAP_REACHED);   
        }
        else
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }
    }

    int flags, set_flags_status;

    flags = fcntl(lock->_pipefd[0], F_GETFL, 0);
    flags |= O_NONBLOCK;

    set_flags_status = fcntl(lock->_pipefd[0], F_SETFL, flags);
    if(set_flags_status != 0)
    {
        gc_lock_destroy(lock, NULL);
        GC_VRETURN(out_err, GC_ERR_UNHANDLED);
    }

    flags = fcntl(lock->_pipefd[1], F_GETFL, 0);
    flags |= O_NONBLOCK;

    set_flags_status = fcntl(lock->_pipefd[1], F_SETFL, flags);
    if(set_flags_status != 0)
    {
        gc_lock_destroy(lock, NULL);
        GC_VRETURN(out_err, GC_ERR_UNHANDLED);
    }
    
    GC_VRETURN(out_err, GC_SUCCESS);
}

void gc_lock_destroy(GCLock* lock, gc_status* out_err)
{
    int close_status;
    while(1)
    {
        close_status = close(lock->_pipefd[0]);
        if(close_status == 0) break; // sucessfully closed
        
        // ELSE

        if(errno == EINTR) continue; // close again, interrupted
        else if(errno == EBADF) break; // already closed
        else 
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }

    }
    while(1)
    {
        close_status = close(lock->_pipefd[1]);
        if(close_status == 0) break; // sucessfully closed
        
        // ELSE

        if(errno == EINTR) continue; // close again, interrupted
        else if(errno == EBADF) break; // already closed
        else
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }
    }

    GC_VRETURN(out_err, GC_SUCCESS);
}

void gc_lock_wait(GCLock* lock, gc_status* out_err)
{
    struct pollfd read_poll_data = {
        .fd = lock->_pipefd[0],
        .events = POLLIN,
    };

    int poll_status;
    while(1)
    {
        poll_status = poll(&read_poll_data, 1, -1);
        if(poll_status > 0) break;

        // ELSE

        if(errno == EINTR) continue; // poll() again.
        else 
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }
    }

    int read_status;
    char a;
    while(1)
    {
        read_status = read(lock->_pipefd[0], &a, 1);
        if(read_status >= 0) continue;

        // ELSE

        if(errno == EAGAIN) break; // successfully read all data
        else if(errno == EINTR) continue; // read() again
        else 
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }
    }

    GC_VRETURN(out_err, GC_SUCCESS);
}

void gc_lock_signal(GCLock* lock, gc_status* out_err)
{
    char a = 1;
    while(1)
    {
        int write_status = write(lock->_pipefd[1], &a, 1);

        if(write_status > 0) break;

        // ELSE

        if(errno == EAGAIN) break; // no more space in buffer, write unnecessary
        else if(errno == EPIPE)
        {
            gc_status err;

            gc_lock_destroy(lock, &err);
            if(err != GC_SUCCESS)
            {
                GC_VRETURN(out_err, GC_ERR_UNHANDLED);
            }

            gc_lock_init(lock, &err);
            if(err != GC_SUCCESS)
            {
                GC_VRETURN(out_err, GC_ERR_UNHANDLED);
            }
            break;
        }
        else
        {
            GC_VRETURN(out_err, GC_ERR_UNHANDLED);
        }
        
    }

    GC_VRETURN(out_err, GC_SUCCESS);
}
