
/**
 * \file elastic_pthread.c
 * Echo stdin to stdout with an arbitrary sized buffer.
 **/
#include "utilace.h"
#include "cx/alphatab.h"
#include "cx/fileb.h"
#include "cx/table.h"

#include <pthread.h>
#include <errno.h>
#include <stdio.h>


typedef struct WritingThreadState WritingThreadState;

struct WritingThreadState
{
  bool done;
  bool waiting;
  pthread_t thread;
  pthread_cond_t buf_cond;
  pthread_mutex_t buf_mutex;
  TableT(byte) buf;
  OFileB ofileb;
};

DeclTableT(WritingThreadState, WritingThreadState);

static
  void
init_WritingThreadState(WritingThreadState* st)
{
  st->done = false;
  st->waiting = true;
  pthread_cond_init(&st->buf_cond, NULL);
  pthread_mutex_init(&st->buf_mutex, NULL);
  InitTable( st->buf );
  init_OFileB(&st->ofileb );
  setfmt_OFileB(&st->ofileb, FileB_Raw);
  st->ofileb.of.mayflush = false;
}

/** Call pthread_join() before this!**/
static
  void
lose_WritingThreadState(WritingThreadState* st)
{
  pthread_cond_destroy(&st->buf_cond);
  pthread_mutex_destroy(&st->buf_mutex);
  LoseTable( st->buf );
  lose_OFileB(&st->ofileb);
}

static
  void
StateMsg(const char* msg, const char* name) {
#if 0
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex);
  fprintf(stderr, "%s %s\n", msg, name);
  pthread_mutex_unlock(&mutex);
#else
  (void)msg;
  (void)name;
#endif
}

static
  void*
writing_thread_fn(WritingThreadState* st) {
  bool done = false;

  while (!done && st->ofileb.fb.good) {
    pthread_mutex_lock(&st->buf_mutex);
    StateMsg("loop start", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
    if (st->buf.sz == 0 && !st->done) {
      st->waiting = true;
      StateMsg("waiting", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
      pthread_cond_wait(&st->buf_cond, &st->buf_mutex);
      StateMsg("done waiting", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
      st->waiting = false;
    }
    done = st->done;
    oputn_byte_OFileB(&st->ofileb, st->buf.s, st->buf.sz);
    FlushTable( st->buf );
    pthread_mutex_unlock(&st->buf_mutex);

    if (!flush_OFileB(&st->ofileb)) {
      break;
    }
  }
  StateMsg("done writing", ccstr_of_AlphaTab(&st->ofileb.fb.filename));

  if (!done) {
    StateMsg("setting myself done", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
    /* The file closed on us!*/
    pthread_mutex_lock(&st->buf_mutex);
    st->done = true;
    pthread_mutex_unlock(&st->buf_mutex);
  }

  close_OFileB(&st->ofileb);
  ClearTable( st->buf );
  StateMsg("end of", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
  return NULL;
}

/** Reading is actually done on the main thread.**/
static
  void
reading_routine(fd_t xfd, TableT(WritingThreadState)* wstates)
{
  static const size_t chunksz = BUFSIZ;
  int istat;
  uint i;
  zuint nspawned = wstates->sz;  /* For early cleanup.*/
  TableT(byte) buf = DEFAULT_Table;

  SizeTable( buf, chunksz );

  for (i = 0; i < wstates->sz; ++i) {
    WritingThreadState* st = &wstates->s[i];
    istat = pthread_create(
        &st->thread, NULL,
        (void* (*) (void*)) writing_thread_fn,
        st);
    if (istat != 0) {
      DBog0("error on pthread_create()");
      nspawned = i;
      break;
    }
  }

  if (nspawned == wstates->sz) {
    while (true) {
      long nbytes = read_sysCx(xfd, buf.s, buf.sz);
      if (nbytes <= 0) {
        break;
      }

      buf.sz = nbytes;
      for (i = 0; i < wstates->sz; ++i) {
        WritingThreadState* st = &wstates->s[i];
        pthread_mutex_lock(&st->buf_mutex);
        if (!st->done) {
          StateMsg("catting", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
          CatTable( st->buf, buf );
          if (st->waiting) {
            pthread_cond_signal(&st->buf_cond);
          }
        }
        pthread_mutex_unlock(&st->buf_mutex);
      }
      buf.sz = chunksz;
    }
  }
  closefd_sysCx(xfd);
  StateMsg("done reading", "input");

  for (i = 0; i < nspawned; ++i) {
    void* ret = NULL;
    WritingThreadState* st = &wstates->s[i];
    pthread_mutex_lock(&st->buf_mutex);
    if (!st->done) {
      StateMsg("setting done", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
      st->done = true;
      if (st->waiting) {
        StateMsg("signaling done", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
        pthread_cond_signal(&st->buf_cond);
      }
    }
    pthread_mutex_unlock(&st->buf_mutex);
    StateMsg("joining", ccstr_of_AlphaTab(&st->ofileb.fb.filename));
    pthread_join(st->thread, &ret);
  }
  LoseTable( buf );
  StateMsg("end of", "input");
}

LaceUtilMain(elastic)
{
  DeclTable( WritingThreadState, wstates );
  const char* xfilename = "/dev/fd/0";
  fd_t xfd = -1;
  uint i;

  /**** BEGIN ARGUMENT_PARSING ****/
  while (argi < argc) {
    const char* arg = argv[argi++];
    WritingThreadState* st;

    if (eq_cstr(arg, "-x")) {
      if (argi == argc) {
        failout_sysCx("Need input file after -x.");
      }
      arg = argv[argi++];

      if (eq_cstr(arg, "-")) {
        xfilename = "/dev/fd/0";
      } else {
        xfilename = arg;
      }
    } else {
      if (eq_cstr(arg, "-o")) {
        if (argi == argc) {
          failout_sysCx("Need input file after -o.");
        }
        arg = argv[argi++];
      }

      st = Grow1Table( wstates );
      init_WritingThreadState(st);

      if (eq_cstr(arg, "-")) {
        arg = "/dev/fd/1";
      }
      if (!open_FileB(&st->ofileb.fb, NULL, arg)) {
        DBog1("failed to open: %s\n", arg);
        failout_sysCx(0);
      }
    }
  }

  xfd = open_lace_xfd(xfilename);
  if (xfd < 0) {
    DBog1("failed to open: %s\n", xfilename);
    failout_sysCx(0);
  }

  if (wstates.sz == 0) {
    WritingThreadState* st = Grow1Table( wstates );
    init_WritingThreadState(st);
    if (!open_FileB(&st->ofileb.fb, NULL, "/dev/fd/1")) {
      failout_sysCx("Failed to open: /dev/stdout");
    }
  }
  /**** END ARGUMENT_PARSING ****/

  reading_routine(xfd, &wstates);
  xfd = -1;

  for (i = 0; i < wstates.sz; ++i) {
    lose_WritingThreadState(&wstates.s[i]);
  }
  LoseTable( wstates );

  lose_sysCx ();
  return 0;
}
