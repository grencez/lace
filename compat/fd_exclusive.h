/* Readers-writer lock for use within fd.c routines.*/

#ifndef _MSC_VER
#include <pthread.h>

#ifdef PTHREAD_RWLOCK_INITIALIZER
static pthread_rwlock_t lace_compat_fd_lock = PTHREAD_RWLOCK_INITIALIZER;
#define LACE_COMPAT_FD_ENTER_SHARED pthread_rwlock_rdlock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_SHARED pthread_rwlock_unlock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_ENTER_EXCLUSIVE pthread_rwlock_wrlock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_EXCLUSIVE pthread_rwlock_unlock(&lace_compat_fd_lock)
#else
static pthread_mutex_t lace_compat_fd_lock = PTHREAD_MUTEX_INITIALIZER;
#define LACE_COMPAT_FD_ENTER_SHARED pthread_mutex_lock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_SHARED pthread_mutex_unlock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_ENTER_EXCLUSIVE pthread_mutex_lock(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_EXCLUSIVE pthread_mutex_unlock(&lace_compat_fd_lock)
#endif

#else
#include <synchapi.h>
static SRWLOCK lace_compat_fd_lock = SRWLOCK_INIT;

#define LACE_COMPAT_FD_ENTER_SHARED AcquireSRWLockShared(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_SHARED ReleaseSRWLockShared(&lace_compat_fd_lock)

#define LACE_COMPAT_FD_ENTER_EXCLUSIVE AcquireSRWLockExclusive(&lace_compat_fd_lock)
#define LACE_COMPAT_FD_LEAVE_EXCLUSIVE ReleaseSRWLockExclusive(&lace_compat_fd_lock)

#endif

