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
 * $Id: free.c,v 1.4 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"

void
mdir_free_params(mdir_param **mdp) {
	mdir_param **kvs;
	mdir_param *kv;

	if(!mdp)
		return;

	for(kvs = mdp; *kvs; kvs++) {
		kv = (mdir_param *)(*kvs);
		if(kv->key)
			free(kv->key);
		if(kv->vals)
			_mdir_list_free(kv->vals);
		free(kv);
	}

	free(mdp);

}

void
mdir_free_line(mdir_line *line) {

	if(!line)
		return;

	if(line->name)
		free(line->name);

	if(line->params)
		mdir_free_params(line->params);

	if(line->values)
		_mdir_list_free(line->values);

	free(line);

};

/*
 * Frees MIME directory entry.
 */
void
mdir_free(mdir_line **mdep) {
	mdir_line **lines;

	if(!mdep)
		return;

	/* Free dependences */
	for(lines = mdep; *lines; lines++)
		mdir_free_line(*lines);

	/* Free the passed structure */
	free(mdep);

}

