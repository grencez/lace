
#include "lace.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

  size_t
read_LaceX(LaceX* in)
{
  if (in->vt && in->vt->read_fn) {
    size_t old_size = in->size;
    in->vt->read_fn(in);
    return in->size - old_size;
  }
  return 0;
}

  void
close_LaceX(LaceX* in)
{
  if (!in) { return; }
  if (in->vt && in->vt->close_fn) {
    in->vt->close_fn(in);
  }
  if (in->at) {
    free(in->at);
    in->at = NULL;
    in->size = 0;
    in->alloc_lgsize = 0;
  }
  in->off = 0;
  if (in->vt && in->vt->free_fn) {
    in->vt->free_fn(in);
  }
}

  char*
grow_LaceX(LaceX* in, size_t capac)
{
  return (char*) grow_LaceA_(
      (void**)&in->at, &in->size, &in->alloc_lgsize,
      1, capac, realloc);
}

  void
flush_LaceX(LaceX* x)
{
  assert(x->off <= x->size);
  if (x->off == 0)  return;
  x->size -= x->off;
  if (x->size > 0) {
    memmove(x->at, &x->at[x->off], x->size);
  }
  x->off = 0;
  if (x->alloc_lgsize == LACE_LGSIZE_MAX) {
    x->at[x->size] = '\0';
  }
}

  void
maybe_flush_LaceX(LaceX* x)
{
  if (x->flush_lgsize == 0) {
    return;
  }
  if (x->off < x->size &&
      (x->off >> x->flush_lgsize) == 0) {
    return;
  }
  flush_LaceX(x);
}

static
  char*
cstr_of_LaceX(LaceX* in)
{
  if (in->alloc_lgsize < LACE_LGSIZE_MAX) {
    *grow_LaceX(in, 1) = '\0';
    in->size -= 1;
  }
  assert(in->at[in->size] == '\0');
  return &in->at[in->off];
}

  char*
slurp_LaceX(LaceX* in)
{
  maybe_flush_LaceX(in);
  while (0 < read_LaceX(in)) {/* Yummy.*/}
  return cstr_of_LaceX(in);
}

/** Like strcspn or strtok but returns a slice.**/
  LaceX
slicechr_LaceX(LaceX* in, const char delim)
{
  LaceX slice = DEFAULT_LaceX;
  size_t ret_off;
  char* s = NULL;

  maybe_flush_LaceX(in);
  ret_off = in->off;
  if (in->size > 0) {
    s = strchr(cstr_of_LaceX(in), delim);
  }

  while (!s) {
    in->off = in->size;
    if (0 == read_LaceX(in)) {
      break;
    }
    s = strchr(cstr_of_LaceX(in), delim);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  slice.at = &in->at[ret_off];
  slice.alloc_lgsize = LACE_LGSIZE_MAX;
  if (s) {
    in->off = 1 + (s - in->at);
    slice.size = in->off - ret_off - 1;
    slice.at[slice.size] = '\0';
  } else {
    assert(in->off == in->size);
    slice.size = in->size - ret_off;
  }
  assert(slice.at[slice.size] == '\0');
  return slice;
}

/** Get line, return slice.**/
  LaceX
sliceline_LaceX(LaceX* in)
{
  LaceX slice = slicechr_LaceX(in, '\n');
  /* Ignore carriage returns.*/
  if (slice.size > 0 && slice.at[slice.size-1] == '\r') {
    slice.size -= 1;
    slice.at[slice.size] = '\0';
  }
  return slice;
}


/** Like strcspn or strtok but returns a slice.**/
  LaceX
slicechrs_LaceX(LaceX* in, const char* delims)
{
  LaceX slice = DEFAULT_LaceX;
  size_t ret_off;
  size_t end;

  maybe_flush_LaceX(in);
  ret_off = in->off;
  end = in->off + strcspn(cstr_of_LaceX(in), delims);

  while (end == in->size) {
    in->off = in->size;
    if (0 == read_LaceX(in)) {
      break;
    }
    end = in->off + strcspn(cstr_of_LaceX(in), delims);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  slice.at = &in->at[ret_off];
  slice.alloc_lgsize = LACE_LGSIZE_MAX;
  if (end < in->size) {
    in->off = end + 1;
    slice.size = in->off - ret_off - 1;
    slice.at[slice.size] = '\0';
  } else {
    assert(in->off == in->size);
    slice.size = in->size - ret_off;
  }
  assert(slice.at[slice.size] == '\0');
  return slice;
}

/** Like strspn but skips.**/
  LaceX
slicespan_LaceX(LaceX* in, const char* span)
{
  LaceX slice = DEFAULT_LaceX;
  size_t ret_off;
  size_t end = in->size;

  maybe_flush_LaceX(in);
  ret_off = in->off;
  if (in->size > 0) {
    end = in->off + strspn(cstr_of_LaceX(in), span);
  }

  while (end == in->size) {
    in->off = in->size;
    if (0 == read_LaceX(in)) {
      break;
    }
    end = in->off + strspn(cstr_of_LaceX(in), span);
  }

  if (ret_off == in->size) {
    assert(in->off == in->size);
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  slice.at = &in->at[ret_off];
  slice.alloc_lgsize = LACE_LGSIZE_MAX;
  if (end < in->size) {
    in->off = end;
    slice.size = in->off - ret_off;
    assert(slice.at[slice.size] != '\0');
  } else {
    assert(in->off == in->size);
    slice.size = in->size - ret_off;
    assert(slice.at[slice.size] == '\0');
  }
  return slice;
}

/** Like strstr but returns a slice.**/
  LaceX
slicestr_LaceX(LaceX* in, const char* delim)
{
  LaceX slice = DEFAULT_LaceX;
  size_t delim_length = strlen(delim);
  char* s = NULL;
  size_t ret_off;

  assert(delim_length > 0);

  maybe_flush_LaceX(in);
  ret_off = in->off;

  while (in->off + delim_length > in->size) {
    if (0 == read_LaceX(in)) {
      break;
    }
  }
  if (in->off + delim_length <= in->size) {
    s = strstr(cstr_of_LaceX(in), delim);
    while (!s) {
      in->off = in->size + 1 - delim_length;
      if (0 == read_LaceX(in)) {
        break;
      }
      s = strstr(cstr_of_LaceX(in), delim);
    }
  }

  if (ret_off == in->size) {
    return slice;  /* Empty.*/
  }
  assert(in->at[in->size] == '\0');

  slice.at = &in->at[ret_off];
  slice.alloc_lgsize = LACE_LGSIZE_MAX;
  if (s) {
    in->off = delim_length + (s - in->at);
    slice.size = in->off - ret_off - delim_length;
    slice.at[slice.size] = '\0';
  } else {
    in->off = in->size;
    slice.size = in->size - ret_off;
  }
  assert(slice.at[slice.size] == '\0');
  return slice;
}

/** Get line, return string.**/
  char*
getline_LaceX(LaceX* in)
{
  LaceX slice = sliceline_LaceX(in);
  return slice.at;
}

/** Get string up to a delimiter. Push offset past delimiter.**/
  char*
gets_LaceX(LaceX* in, const char* delim)
{
  LaceX slice = slicestr_LaceX(in, delim);
  return slice.at;
}

  void
skipchrs_LaceX(LaceX* in, const char* span)
{
  slicespan_LaceX(in, span);
  maybe_flush_LaceX(in);
}

static const char lace_whitespace[] = " \t\v\r\n";

  bool
parse_int_LaceX(LaceX* in, int* ret)
{
  LaceX slice;
  char* end = NULL;
  skipchrs_LaceX(in, lace_whitespace);
  slice = slicespan_LaceX(in, "+-0123456789");
  if (slice.size > 0) {
    end = lace_parse_int(ret, slice.at);
  }
  return !!end;
}

  bool
parse_double_LaceX(LaceX* in, double* ret)
{
  LaceX slice;
  char* end = NULL;
  skipchrs_LaceX(in, lace_whitespace);
  slice = slicespan_LaceX(in, "+-.0123456789Ee");
  if (slice.size > 0) {
    end = lace_parse_double(ret, slice.at);
  }
  return !!end;
}
