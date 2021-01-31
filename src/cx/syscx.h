/**
 * \file syscx.h
 **/
#ifndef sysCx_H_
#define sysCx_H_

static const char MagicArgv1_sysCx[] = "--opts://sysCx";

const char*
exename_of_sysCx ();
int
init_sysCx (int* pargc, char*** pargv);
void
push_losefn_sysCx (void (*f) ());
void
push_losefn1_sysCx (void (*f) (void*), void* x);
void
lose_sysCx ();

/* synhax.h - failout_sysCx() */
/* synhax.h - dbglog_printf3() */
/* fileb.h - stdin_XFileB () */
/* fileb.h - stdout_OFileB () */
/* fileb.h - stderr_OFileB () */

#if defined(_WIN32)
#elif defined(__APPLE__)
#define LACE_POSIX_SOURCE
#else
#define LACE_POSIX_SOURCE
/* TODO: Figure out the correct POSIX_SOURCE to use!*/
#ifndef POSIX_SOURCE
#define POSIX_SOURCE
#endif
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
/* Needed by setenv().*/
#define _POSIX_C_SOURCE 200112L
#endif
#endif

#ifdef LACE_POSIX_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#else
#include <fcntl.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <winsock2.h>
#ifdef _MSC_VER
typedef intptr_t pid_t;
#endif
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

typedef int fd_t;

#include "def.h"

bool
pipe_sysCx (fd_t* fds);
fd_t
dup_sysCx (fd_t fd);
bool
dup2_sysCx (fd_t oldfd, fd_t newfd);
int
open_lace_xfd(const char* filename);
int
open_lace_ofd(const char* filename);
long
read_sysCx (fd_t fd, void* buf, long sz);
long
write_sysCx (fd_t fd, const void* buf, long sz);
bool
closefd_sysCx (fd_t fd);
FILE*
fdopen_sysCx (fd_t fd, const char* mode);
int
poll_sysCx(struct pollfd* pollfds, size_t npollfds, int timeout);
pid_t
spawnvp_sysCx (char* const* argv);
void
execvp_sysCx (char* const* argv);
bool
waitpid_sysCx (pid_t pid, int* status);

void
setenv_sysCx (const char* key, const char* val);
void
tacenv_sysCx (const char* key, const char* val);
void
cloexec_sysCx (fd_t fd, bool b);

bool
chmodu_sysCx (const char* pathname, bool r, bool w, bool x);
bool
mkdir_sysCx (const char* pathname);
bool
rmdir_sysCx (const char* pathname);
bool
chdir_sysCx (const char* pathname);
Bool
randomize_sysCx(void* p, uint size);

#endif

