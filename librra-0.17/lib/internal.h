/* $Id: internal.h 779 2003-05-18 16:38:27Z twogood $ */
#ifndef __internal_h__
#define __internal_h__

#include "../rra_config.h"

#include <stddef.h>

#if !HAVE_STRNDUP
char *rra_strndup (const char *s, size_t n);
#define strndup rra_strndup
#endif

#if !HAVE_STRCASESTR
char *rra_strcasestr(const char *haystack, const char *needle);
#define strcasestr rra_strcasestr
#endif

#endif
