
#include "cx/fileb.h"
#include "cx/sys-cx.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char* ExeName = 0;
#define ErrOut stderr

static void
show_usage_and_exit ()
{
    const char* s;
    s = "Usage: %s TABLE QUERIES [OPTION]*\n";
    fprintf (ErrOut, s, ExeName);
    s = "    TABLE is a file used for lookup.\n";
    fputs (s, ErrOut);
    s = "    QUERIES is a file, each line prompts an output of the closest match from TABLE.\n";
    fputs (s, ErrOut);
    exit (1);
}

    /** Open a file for reading or writing.
     * Exit with an error status if this is not possible.
     **/
static FILE*
open_file_arg (const char* arg, bool writing)
{
    FILE* f;
    if (writing)
    {
        if (0 == strcmp (arg, "-"))  f = stdout;
        else                         f = fopen (arg, "wb");
    }
    else
    {
        if (0 == strcmp (arg, "-"))  f = stdin;
        else                         f = fopen (arg, "rb");
    }
    if (!f)
    {
        fprintf (ErrOut, "%s - Cannot open file for %s:%s\n",
                 ExeName,
                 writing ? "writing" : "reading",
                 arg);
        exit (1);
    }
    return f;
}

static char**
split_lines (char* buf, uint* ret_max_len)
{
    uint i;
    uint n = 1;
    uint max_len = 1;
    char* s = buf;
    char** lines;

    while (1)
    {
        s = strchr (s, '\n');
        if (!s)  break;
        ++n;
        s[0] = '\0';
        s = &s[1];
    }

    lines = AllocT( char*, n+1 );
    lines[n] = 0;

    s = buf;
    for (i = 0; i < n; ++i)
    {
        uint len;
        lines[i] = s;
        len = strlen (s);
        if (len > max_len)  max_len = len;
        s = &s[1 + len];
    }

    *ret_max_len = max_len;
    return lines;
}

    /** Find the length of the longest common subsequence between /x/ and /y/.
     * /width/ of /a/ must not be less than the length of /y/.
     **/
static uint
lcs_count (uint* a, uint width, const char* x, const char* y)
{
    uint i, j, m, n;

    memset (a, 0, width * sizeof (uint));
    m = strlen (x);
    n = strlen (y);
    assert (n <= width);

    if (m == 0 || n == 0)  return 0;

    for (i = 0; i < m; ++i)
    {
        uint back_j = 0, back_ij = 0;
        char xc;
        xc = tolower (x[i]);
        for (j = 0; j < n; ++j)
        {
            uint back_i;
            char yc;
            yc = tolower (y[j]);
            back_i = a[j];

            if (xc == yc)
                a[j] = back_ij + 1;
            else if (back_j > back_i)
                a[j] = back_j;

            back_ij = back_i;
            back_j = a[j];
        }
    }

    return a[n-1];
}

static uint
matching_line (uint* a, uint width, const char* s, char* const* lines)
{
    uint max_count = 0;
    uint match_idx = 0;
    uint i;

    for (i = 0; lines[i]; ++i)
    {
        uint count;
        count = lcs_count (a, width, s, lines[i]);
        if (count > max_count)
        {
            max_count = count;
            match_idx = i;
        }
    }
    return match_idx;
}

int main (int argc, char** argv)
{
    uint* lcs_array;
    FileB lookup_in;
    FileB stream_in;
    FILE* out;
    char* buf;
    char* s;
    char** lines;
    uint width;
    int argi = 1;

    init_sys_cx ();
    ExeName = argv[0];

    init_FileB (&lookup_in);
    init_FileB (&stream_in);

    if (argi >= argc)
        show_usage_and_exit ();

    lookup_in.f = open_file_arg (argv[argi++], false);

    if (argi >= argc)
        show_usage_and_exit ();

    buf = load_FileB (&lookup_in);
    lines = split_lines (buf, &width);
    lcs_array = AllocT( uint, width );

    stream_in.f = open_file_arg (argv[argi++], false);

    while (argi < argc)
    {
        const char* arg = argv[argi];
        ++ argi;
        if (0 == strcmp (arg, "-h"))
        {
            show_usage_and_exit ();
        }
        else
        {
            fprintf (ErrOut, "%s - Unknown argument:%s\n", ExeName, arg);
            show_usage_and_exit ();
        }
    }

    out = stdout;
    for (s = getline_FileB (&stream_in);
         s;
         s = getline_FileB (&stream_in))
    {
        uint i;
        i = matching_line (lcs_array, width, s, lines);
        fputs (lines[i], out);
        fputc ('\n', out);
    }

    free (lcs_array);
    free (lines);
    lose_FileB (&lookup_in);
    lose_FileB (&stream_in);
    lose_sys_cx ();
    return 0;
}

