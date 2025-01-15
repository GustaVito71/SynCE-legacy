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
 * $Id: dup.c,v 1.3 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"

char **
mdir_dup_list(char **olist) {
	char **list;
	size_t size;

	if(!olist)
		return NULL;

	size = mdir_count((void **)olist);

	list = (char **)malloc((size + 1) * sizeof(char *));
	if(!list)
		return NULL;

	list[size] = NULL;

	for(size = 0; *olist; size++, olist++) {
		if(!(list[size] = strdup(*olist))) {
			_mdir_list_free(list);
			return NULL;
		}
	}

	return list;
}

mdir_param *
mdir_dup_param(mdir_param *mdp) {
	mdir_param *mdpn;

	if(!mdp)
		return NULL;

	mdpn = (mdir_param *)calloc(1, sizeof(mdir_param));
	if(!mdpn) {
		return NULL;
	}

	if(mdp->key) {
		if(!(mdpn->key = strdup(mdp->key))) {
			free(mdpn);
			return NULL;
		}
	}

	if(mdp->vals) {
		if(!(mdpn->vals = mdir_dup_list(mdp->vals))) {
			if(mdp->key)
				free(mdpn->key);
			free(mdpn);
			return NULL;
		}
	}

	return mdpn;
}

mdir_param **
mdir_dup_params(mdir_param **mdp) {
	mdir_param **mdpn;
	size_t size;

	if(!mdp)
		return NULL;

	/* Count elements in mdp */
	size = mdir_count((void **)mdp);
	
	mdpn = (mdir_param **)malloc((size + 1) * sizeof(mdir_param *));
	if(!mdpn)
		return NULL;

	mdpn[size]=NULL;

	for(size=0; *mdp; size++, mdp++) {
		if(!(mdpn[size] = mdir_dup_param(*mdp))) {
			mdir_free_params(mdpn);
			return NULL;
		}
	}

	return mdpn;
}


mdir_line *
mdir_dup_line(mdir_line *mdep) {
	mdir_line *mdn;

	if(!mdep)
		return NULL;

	mdn = (mdir_line *)calloc(1, sizeof(mdir_line));
	if(!mdn) {
		return NULL;
	}

	if(mdep->name) {
		mdn->name = strdup(mdep->name);
		if(!mdn->name) {
			mdir_free_line(mdn);
			return NULL;
		}
	}

	if(mdep->params) {
		mdn->params = mdir_dup_params(mdep->params);
		if(!mdn->params) {
			mdir_free_line(mdn);
			return NULL;
		}
	}

	if(mdep->values) {
		mdn->values = mdir_dup_list(mdep->values);
		if(!mdn->values) {
			mdir_free_line(mdn);
			return NULL;
		}
	}

	return mdn;
}




mdir_line **
mdir_dup(mdir_line **mdep) {
	mdir_line **mdn;
	size_t size;

	if(!mdep)
		return NULL;

	/* Count elements in mdep */
	size = mdir_count((void **)mdep);

	mdn = (mdir_line **)malloc((size + 1) * sizeof(mdir_line *));

	/* Null-terminate */
	mdn[size] = NULL;

	for(size=0; *mdep; size++, mdep++) {
		if(!(mdn[size] = mdir_dup_line(*mdep))) {
			mdir_free(mdn);
			return NULL;
		}
	}

	return mdn;
}
