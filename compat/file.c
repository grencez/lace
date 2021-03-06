#include "lace_compat_file.h"

#include <stdlib.h>
#include <string.h>

  const char*
lace_compat_file_basename(const char* filepath)
{
  const char* s;
  if (!filepath) {return NULL;}
  s = strrchr(filepath, '/');
  if (s) {filepath = &s[1];}
#ifdef _WIN32
  s = strrchr(filepath, '\\');
  if (s) {filepath = &s[1];}
  s = strrchr(filepath, ':');
  if (s) {filepath = &s[1];}
#endif
  return filepath;
}

  char*
lace_compat_file_abspath(const char* filepath)
{
#ifdef _WIN32
  return _fullpath(NULL, filepath, _MAX_PATH);
#else
  /* This also resolves symlinks. Whatever.*/
  return realpath(filepath, NULL);
#endif
}

  char*
lace_compat_file_catpath(const char* dir, const char* filename)
{
  const size_t dir_length = (dir ? strlen(dir) : 0);
  const size_t add_length = (filename ? strlen(filename) : 0);
  size_t i;
  char* p;
  if (dir_length + add_length == 0) {return NULL;}

  p = (char*) malloc(dir_length + 1 + add_length + 1);
  i = 0;
  memcpy(&p[i], dir, dir_length);
  i += dir_length;
  if (dir_length > 0 && dir[dir_length-1] != '/') {
    p[i++] = '/';
  }
  memcpy(&p[i], filename, add_length);
  i += add_length;
  p[i] = '\0';
  return p;
}
