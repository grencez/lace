
/** Simple utility to transpose based on a delimiter.**/

#include "lace.h"
#include "lace_compat_string.h"
#include "cx/alphatab.h"
#include "cx/table.h"

  int
main_transpose(unsigned argc, char** argv)
{
  DeclTableT( cstr_row, TableT(cstr) );
  DeclTable( cstr_row, mat );
  LaceX* in;
  LaceO* out;
  LaceX slice;
  const char* const delim = argv[1];
  DeclTable( uint, row_widths );
  unsigned i;
  unsigned ncols = 0;

  if (argc != 2 || !delim) {
    lace_log_error("Need exactly one argument.");
    return 64;
  }

  in = open_fd_LaceX(0);
  for (slice = sliceline_LaceX(in);
       slice.at;
       slice = sliceline_LaceX(in))
  {
    const char* field;
    size_t max_width = 0;
    TableT(cstr)* row = Grow1Table( mat );
    InitTable(*row);

    skipchrs_LaceX(&slice, " ");
    for (field = gets_LaceX(&slice, delim);
         field;
         field = gets_LaceX(&slice, delim))
    {
      size_t width = strlen(field);
      if (width > max_width) {
        max_width = width;
      }
      PushTable( *row, lace_compat_string_duplicate(field) );
      if (row->sz > ncols) {
        ncols = row->sz;
      }
      skipchrs_LaceX(&slice, " ");
    }
    if (row_widths.sz > 0) {
      max_width += 1;  /* 1 extra space after delim in the output.*/
    }
    PushTable( row_widths, max_width );
  }
  close_LaceX(in);

  out = open_fd_LaceO(1);
  for (i = 0; i < ncols; ++i) {
    unsigned j;
    for (j = 0; j < mat.sz; ++j) {
      char* field = (i < mat.s[j].sz ? mat.s[j].s[i] : NULL);
      size_t field_width = field ? strlen(field) : 0;
      size_t width_needed;
      assert(field_width <= row_widths.s[j]);
      if (field || j + 1 < mat.sz) {
        for (width_needed = row_widths.s[j] - field_width;
             width_needed > 0;
             width_needed -= 1)
        {
          putc_LaceO(out, ' ');
        }
      }

      if (field) {
        puts_LaceO(out, field);
        free(field);
      }
      if (j + 1 < mat.sz) {
        puts_LaceO(out, delim);
      }
    }
    putc_LaceO(out, '\n');
  }
  close_LaceO(out);

  LoseTable( row_widths );
  for (i = 0; i < mat.sz; ++i) {
    LoseTable( mat.s[i] );
  }
  LoseTable( mat );
  return 0;
}

#ifndef LACE_BUILTIN_LIBRARY
  int
main(int argc, char** argv)
{
  int istat = main_transpose(argc, argv);
  return istat;
}
#endif
