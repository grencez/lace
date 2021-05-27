#ifndef LACE_H_
#define LACE_H_
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus) || defined(__OPENCL_VERSION__)
/* These already have bool.*/
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#elif !defined(true)
# define true 1
# define false 0
typedef char bool;
# define bool bool
#endif

typedef int lace_fd_t;
typedef uint8_t lace_lgsize_t;
#define LACE_LGSIZE_MAX UCHAR_MAX

typedef struct LaceX_VTable LaceX_VTable;
typedef struct LaceX LaceX;
typedef struct LaceXF LaceXF;
typedef struct LaceO_VTable LaceO_VTable;
typedef struct LaceO LaceO;
typedef struct LaceOF LaceOF;


size_t read_LaceX(LaceX*);
void close_LaceX(LaceX*);
char* grow_LaceX(LaceX*, size_t);
void flush_LaceX(LaceX*);
void maybe_flush_LaceX(LaceX*);
LaceX slicechr_LaceX(LaceX*, const char delim);
LaceX sliceline_LaceX(LaceX*);
LaceX slicechrs_LaceX(LaceX*, const char* delims);
LaceX slicespan_LaceX(LaceX*, const char* span);
LaceX slicestr_LaceX(LaceX*, const char* delim);
char* getline_LaceX(LaceX*);
char* gets_LaceX(LaceX*, const char* delim);
void skipchrs_LaceX(LaceX*, const char* span);
bool parse_int_LaceX(LaceX*, int*);
bool parse_double_LaceX(LaceX*, double*);

bool open_LaceXF(LaceXF* f, const char* filename);
bool open_sibling_LaceXF(LaceXF* f, const char* sibling, const char* filename);

char* lace_parse_int(int* ret, const char* in);
char* lace_parse_double(double* ret, const char* in);

size_t write_LaceO(LaceO*);
void close_LaceO(LaceO*);
char* grow_LaceO(LaceO*, size_t);
void flush_LaceO(LaceO*);
void maybe_flush_LaceO(LaceO*);
void putc_LaceO(LaceO*, char);
void puts_LaceO(LaceO*, const char*);
void print_int_LaceO(LaceO*, int);
void print_double_LaceO(LaceO*, double);

bool open_LaceOF(LaceOF* f, const char* filename);
bool open_sibling_LaceOF(LaceOF* f, const char* sibling, const char* filename);


/** Given the memory address of a structure's field,
 * get the address of the structure.
 * \param T      Type.
 * \param field  Name of the field.
 * \param p      Memory address of the field.
 *
 * lace_castup( T, field, p ) == container_of(p, T, field)
 **/
#define lace_castup( T, field, p ) \
  ((T*) ((uintptr_t) (p) - (ptrdiff_t) offsetof(T, field)))


struct LaceX_VTable
{
  void (*read_fn)(LaceX*);
  void (*close_fn)(LaceX*);
};
#define DEFINE_LaceX_VTable(T, field) \
  static void read_##T##_LaceX(LaceX* input) { \
    read_##T(lace_castup(T, field, input)); \
  } \
  static void close_##T##_LaceX(LaceX* input) { \
    close_##T(lace_castup(T, field, input)); \
  } \
  static const LaceX_VTable DEFAULT_##T##_LaceX_VTable[1] = {{ \
    read_##T##_LaceX, \
    close_##T##_LaceX, \
  }}

struct LaceX {
  char* at;
  size_t size;
  size_t off;
  lace_lgsize_t alloc_lgsize;
  lace_lgsize_t flush_lgsize;
  const LaceX_VTable* vt;
};
#define DEFAULT_LaceX  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_LaceX(vt)  { NULL, 0, 0, 0, 12, vt }

struct LaceXF {
  LaceX base;
  lace_fd_t fd;
  /* unsigned basename_offset; */
  char* filename;
};
#define DEFAULT_LaceXF  { DEFAULT_LaceX, -1, NULL }

struct LaceO_VTable
{
  void (*write_fn)(LaceO*);
  void (*close_fn)(LaceO*);
};
#define DEFINE_LaceO_VTable(T, field) \
  static void write_##T##_LaceO(LaceO* input) { \
    write_##T(lace_castup(T, field, input)); \
  } \
  static void close_##T##_LaceO(LaceO* input) { \
    close_##T(lace_castup(T, field, input)); \
  } \
  static const LaceO_VTable DEFAULT_##T##_LaceO_VTable[1] = {{ \
    write_##T##_LaceO, \
    close_##T##_LaceO, \
  }}

struct LaceO {
  char* at;
  size_t size;
  size_t off;
  lace_lgsize_t alloc_lgsize;
  lace_lgsize_t flush_lgsize;
  const LaceO_VTable* vt;
};
#define DEFAULT_LaceO  { NULL, 0, 0, 0, 12, NULL }
#define DEFAULT1_LaceO(vt)  { NULL, 0, 0, 0, 12, vt }

struct LaceOF {
  LaceO base;
  lace_fd_t fd;
  char* filename;
};
#define DEFAULT_LaceOF  { DEFAULT_LaceO, -1, NULL }


#if !defined(ZEROIZE)
# define ZEROIZE(x)  memset(&(x), 0, sizeof(x))
#endif

#if __STDC_VERSION__ < 199901L && !defined(inline)
#define inline __inline
#endif

static inline
  void*
grow_LaceA_(void** at, size_t* count, lace_lgsize_t* allocated_lgcount,
            size_t element_size, size_t capac,
            void* (*realloc_fn) (void*, size_t))
{
  *count += capac;
  if (*allocated_lgcount < LACE_LGSIZE_MAX && /* Allocation is not fixed.*/
      (*count << 1) > ((size_t)1 << *allocated_lgcount)) {
    void* p;
    if (*allocated_lgcount == 0) {
      *at = 0;
      *allocated_lgcount = 1;
    }
    while (*count > ((size_t)1 << *allocated_lgcount)) {
      *allocated_lgcount += 1;
    }

    *allocated_lgcount += 1;
    p = realloc_fn(*at, element_size << (*allocated_lgcount - 1));
    if (!p) {
      *count -= capac;
      return NULL;
    }
    *at = p;
  }
  return (void*)((uintptr_t)*at + (*count - capac) * element_size);
}

static inline
  void
mpop_LaceA_(void** at, size_t* count, lace_lgsize_t* allocated_lgcount,
            size_t element_size, size_t capac,
            void* (*realloc_fn) (void*, size_t))
{
  *count -= capac;
  if (*allocated_lgcount == LACE_LGSIZE_MAX) {
    /* This allocation is fixed. */
    return;
  }
  if ((*allocated_lgcount >= 3) && ((*count >> (*allocated_lgcount - 3)) == 0)) {
    while ((*allocated_lgcount >= 4) && ((*count >> (*allocated_lgcount - 4)) == 0))
      *allocated_lgcount -= 1;
    *allocated_lgcount -= 1;

    if (*allocated_lgcount > 0) {
      *at = realloc_fn(*at, element_size << (*allocated_lgcount - 1));
    } else {
      *at = realloc_fn(*at, 0);
    }
  }
}

#endif
