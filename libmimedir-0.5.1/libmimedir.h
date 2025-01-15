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
 * $Id: libmimedir.h,v 1.5 2005/03/03 20:35:26 vlm Exp $
 */

#ifndef	__LIBMIMEDIR_H__
#define	__LIBMIMEDIR_H__

#include <stdio.h>

/*
 * MIME Directory Information type definitions.
 */

typedef struct {
	char *key;
	char **vals;
} mdir_param;

typedef struct {
	char *name;
	mdir_param **params;
	char **values;
} mdir_line;

/*
 * Parse specified MIME Directory source
 */

mdir_line **mdir_parse(char *block);
mdir_line **mdir_parse_file(char *filename);
mdir_line **mdir_parse_FILE(FILE *f);

/*
 * Add or insert content line
 */
mdir_line **mdir_add(mdir_line **optMdir, mdir_line *line);
mdir_line **mdir_insert(mdir_line **optMdir, mdir_line *line, int pos);
mdir_line **mdir_concat(mdir_line **with, mdir_line **from);

/*
 * Delete content line
 */
int mdir_delete(mdir_line **mdir, int pos);
int mdir_delete_byname(mdir_line **mdir, char *name);

/*
 * Count list entries
 */
size_t mdir_count(void **);

/*
 * Duplicate MIME Directory Profile structure
 */
mdir_line **mdir_dup(mdir_line **);
mdir_line *mdir_dup_line(mdir_line *);


/*
 * Optional helper functions.
 */

char *mdir_get_value_byname(mdir_line **, char *name);
char **mdir_get_values_byname(mdir_line **, char *name);

/* Get entry line */
mdir_line *mdir_get_line(mdir_line **, char *name);

/* Get entry parameters */
char **mdir_get_params(mdir_line *);

/* And the values of the given parameter */
char **mdir_get_param_values(mdir_line *, char *paramName);

/* Get all values of specified line */
char **mdir_get_line_values(mdir_line *);


/*
 * Free the allocated storage
 */
void mdir_free(mdir_line **);
void mdir_free_line(mdir_line *);
void mdir_free_params(mdir_param **);


#endif

