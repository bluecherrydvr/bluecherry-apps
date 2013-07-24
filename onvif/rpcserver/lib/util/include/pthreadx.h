/* Copyright (C) 2001 by First Peer, Inc. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission. 
**  
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE. */

#ifndef PTHREADX_H_INCLUDED
#define PTHREADX_H_INCLUDED

#include "xmlrpc_config.h"

#if HAVE_PTHREAD
#  define _REENTRANT
#  include <pthread.h>
#elif HAVE_WINDOWS_THREAD
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef HANDLE pthread_t;
typedef CRITICAL_SECTION pthread_mutex_t;

#define PTHREAD_MUTEX_INITIALIZER NULL
    /* usage: pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; */

typedef
struct {
    int attrs; /* currently unused. placeholder. */
} pthread_attr_t;

typedef
struct {
    int attrs; /* currently unused. placeholder. */
} pthread_mutexattr_t;

typedef void * pthread_func(void *);

extern int pthread_create(pthread_t *            const new_thread_ID,
                          const pthread_attr_t * const attr,
                          pthread_func * start_func,
                          void *                 const arg);
extern int pthread_cancel(pthread_t target_thread);
extern int pthread_join(pthread_t target_thread, void **status);
extern int pthread_detach(pthread_t target_thread);

extern int pthread_mutex_init(pthread_mutex_t *           const mp,
                              const pthread_mutexattr_t * const attr);
extern int pthread_mutex_lock(pthread_mutex_t * const mp);
extern int pthread_mutex_unlock(pthread_mutex_t * const mp);
extern int pthread_mutex_destroy(pthread_mutex_t * const mp);

#ifdef __cplusplus
}
#endif
#else  /* HAVE_WINDOWS_THREAD */
  #error "You don't have any thread facility.  (According to "
  #error "HAVE_PTHREAD and HAVE_WINDOWS_THREAD macros defined in "
  #error "xmlrpc_config.h)"
#endif

#endif
