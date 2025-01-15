/*-
 * Copyright (c) 2001 Lev Walkin <vlm@lionet.info>.
 * All rights reserved.
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
 * $Id: parse.c,v 1.5 2005/03/03 20:35:27 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"

extern FILE *_mdir_in;
void *_mdir__scan_string(void *);
void _mdir__delete_buffer(void *);
void _mdir_restart(FILE *);

int _mdir_lex();

int _mdir_parse(mdir_line ***);

mdir_line **
mdir_parse(char *block) {
	mdir_line **mde;
	void *ybuf;

	if(!block) {
		errno = EINVAL;
		return NULL;
	}

	ybuf = _mdir__scan_string(block);

	mde = NULL;
	if(_mdir_parse(&mde)) {
		_mdir__delete_buffer(ybuf);
		return NULL;
	}

	_mdir__delete_buffer(ybuf);

	return mde;
}

mdir_line **
mdir_parse_file(char *filename) {
	FILE *f;
	mdir_line **mde;

	if(!filename) {
		errno = EINVAL;
		return NULL;
	}

	f = fopen(filename, "r");
	if(!f) {
		/* ENOENT? */
		return NULL;
	}

	mde = mdir_parse_FILE(f);

	fclose(f);

	return mde;
}

mdir_line **
mdir_parse_FILE(FILE *f) {
	mdir_line **mde;

	if(!f) {
		errno = EINVAL;
		return NULL;
	}

	_mdir_restart(f);

	mde = NULL;
	if(_mdir_parse(&mde))
		return NULL;

	return mde;
}

