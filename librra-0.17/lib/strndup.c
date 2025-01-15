/* $Id: strndup.c 4107 2012-10-04 15:34:18Z mark_ellis $ */
#include "internal.h"
#include <stdlib.h>
#include <string.h>

char *rra_strndup (const char *s, size_t n)
{
  char *r;

  if (!s)
    return NULL;

  if (strlen (s) < n)
    n = strlen (s);

  r = malloc (n + 1);
  memcpy (r, s, n);
  r[n] = '\0';
  return r;
}

