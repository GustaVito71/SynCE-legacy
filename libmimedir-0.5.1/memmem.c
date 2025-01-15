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
 * $Id: memmem.c,v 1.3 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"

struct mdir_memchunk {
	void *p;
	struct mdir_memchunk *next;
};


int
_mdir_mem_replacechunk(struct mdir_memchunk *mdm, void *op, void *p) {
	if(!mdm)
		return -1;

	if(mdm->p == op) {
		mdm->p = p;
		return 0;
	}

	return _mdir_mem_replacechunk(mdm->next, op, p);
}

void *
_mdir_mem_realloc(void **arg, void *op, size_t size) {
	void *p;

	p = realloc(op, size);

	if(op != p)
		_mdir_mem_replacechunk((struct mdir_memchunk *)*arg, op, p);

	return p;
}

int
_mdir_mem_forgetchunk2(struct mdir_memchunk *mdm, void *p) {
	if(!mdm)
		return -1;

	if(mdm->p == p) {
		mdm->p = NULL;
		return 0;
	}

	return _mdir_mem_forgetchunk2(mdm->next, p);
}

void
_mdir_mem_forgetchunk(void **arg, void *p) {
	if(_mdir_mem_forgetchunk2((struct mdir_memchunk *)*arg, p)) {
		fprintf(stderr, "forgetchunk() on not allocated data: %p\n", p);
		abort();
	}
}

void
_mdir_mem_free(void **arg, void *p) {
	if(_mdir_mem_forgetchunk2((struct mdir_memchunk *)*arg, p)) {
		fprintf(stderr, "free() on not allocated data: %p\n", p);
		abort();
	} else {
		free(p);
	}
}


int
_mdir_mem_addchunk(void **arg, void *p) {
	struct mdir_memchunk *mch;

	mch = (struct mdir_memchunk *)malloc(sizeof(struct mdir_memchunk));
	if(!mch)
		return -1;

	/* Add to the head */
	mch->next = *arg;
	mch->p = p;
	*arg = mch;

	return 0;
};

void
_mdir_mem_forget2(struct mdir_memchunk *mdm, int freeit) {
	if(!mdm)
		return;

	if(freeit && mdm->p) {
		free(mdm->p);
	}

	/* One test or every chunk test? To be, or not to be? */
	_mdir_mem_forget2(mdm->next, freeit);

	free(mdm);
};

void
_mdir_mem_forget(void **arg, int freeit) {
	_mdir_mem_forget2((struct mdir_memchunk *)*arg, freeit);
	*(void **)arg = NULL;
};

void *
_mdir_mem_malloc(void **arg, size_t size) {
	void *p;

	p = malloc(size);
	if(!p)
		return NULL;

	if(_mdir_mem_addchunk(arg, p)) {
		free(p);
		return NULL;
	};

	return p;
};

char *
_mdir_mem_strdup(void **arg, char *str) {
	char *p;

	p = strdup(str);
	if(!p) return NULL;

	if(_mdir_mem_addchunk(arg, p)) {
		free(p);
		return NULL;
	};

	return p;
};

char *
_mdir_mem_stradd(void **arg, char *src, char *str) {
	size_t len;
	char *p;

	len = strlen(src);

	p = (char *)_mdir_mem_realloc(arg, src, len + strlen(str) + 1);
	if(!p) {
		_mdir_mem_free(arg, src);
		return NULL;
	}

	strcpy(p + len, str);

	return p;
}

size_t
_mdir_count_values(char **list) {
	size_t z;

	if(!list)
		return 0;

	for(z = 0; *list; list++, z++);

	return z;
}

char **
_mdir_mem_list_extend(void **arg, char ***list) {
	char **p;
	size_t cv;

	if(!list) {
		p = (char **)_mdir_mem_malloc(arg, sizeof(char *) << 1);
		p[0] = NULL;
		p[1] = NULL;
		return p;
	}

	cv = _mdir_count_values(*list);
	p = (char **)_mdir_mem_realloc(arg, *list, (cv + 2) * sizeof(char *));
	if(!p)
		return NULL;

	p[cv + 1] = NULL;
	*list = p;

	return p + cv;
}

void
_mdir_list_free(char **list) {
	char **vp;
	for(vp = list; *vp; vp++)
		free(*vp);
	free(list);
}


