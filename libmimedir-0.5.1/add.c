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
 * $Id: add.c,v 1.2 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"


mdir_line **
mdir_insert(mdir_line **mde, mdir_line *line, int pos) {
	size_t count;
	int i;

	if(!line || !line->name)
		return NULL;

	count = mdir_count((void **)mde);

	mde = (mdir_line **)realloc(mde, (count + 2) * sizeof(mdir_line *));
	if(!mde)
		return NULL;

	while(pos < 0)
		pos = count + pos + 1;
	if(pos > count)
		pos = count;

	for(i = count + 1; i > pos; i--)
		mde[i] =  mde[i-1];

	mde[pos] = line;

	return mde;
}

mdir_line **
mdir_add(mdir_line **mde, mdir_line *line) {
	return mdir_insert(mde, line, -1);
}


mdir_line **
mdir_concat(mdir_line **line1, mdir_line **line2) {
	mdir_line **ml;
	size_t count1;
	size_t count2;

	if(!line1 || !line2)
		return NULL;

	count1 = mdir_count((void *)line1);
	count2 = mdir_count((void *)line2);

	line1 = (mdir_line **)realloc(line1,
		(count1 + count2 + 1) * sizeof(mdir_line *));
	line1[count1 + count2] = NULL;

	for(ml = line2; *ml; ml++, count1++)
		line1[count1] = *ml;

	free(line2);
	return line1;
}
