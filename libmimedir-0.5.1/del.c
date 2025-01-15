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
 * $Id: del.c,v 1.3 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"

int
mdir_delete(mdir_line **mdir, int pos) {
	size_t count;

	if(!mdir)
		return -1;

	count = mdir_count((void **)mdir);
	if(!count)
		return 0;

	while(pos < 0)
		pos = count + pos;
	if(pos > count)
		pos = count;

	mdir_free_line(mdir[pos]);

	for(; pos < count; pos++)
		mdir[pos] = mdir[pos + 1];

	return 0;
}

int
mdir_delete_byname(mdir_line **mdir, char *name) {

	if(!mdir || !name)
		return -1;

	for(; *mdir; mdir++) {
		if(strcasecmp(name, (*mdir)->name))
			continue;

		mdir_free_line(*mdir);
		mdir_delete_byname(mdir + 1, name);
		for(; *mdir; mdir++)
			*mdir = mdir[1];

		return 0;
	}

	return 0;
}
