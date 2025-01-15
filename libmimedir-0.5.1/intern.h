/*-
 * Copyright (c) 2001 Lev Walkin <vlm@lionet.info>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: intern.h,v 1.4 2005/03/03 20:35:26 vlm Exp $
 */

#ifndef	__INTERNAL_H__
#define	__INTERNAL_H__

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef	HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef	STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef	HAVE_STRING_H
#include <string.h>
#endif

#include <errno.h>

#include "libmimedir.h"

void *_mdir_mem_malloc(void **memcache, size_t size);
char *_mdir_mem_strdup(void **memcache, char *str);
int _mdir_mem_addchunk(void **memcache, void *p);
void _mdir_mem_forgetchunk(void **memcache, void *p);
void _mdir_mem_forget(void **memcache, int freeit);


char *_mdir_mem_stradd(void **memcache, char *src, char *str);

char **_mdir_mem_list_extend(void **memcache, char ***list);
void _mdir_list_free(char **list);

#endif
