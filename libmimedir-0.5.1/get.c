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
 * $Id: get.c,v 1.4 2005/03/03 20:35:26 vlm Exp $
 */

#include "intern.h"
#include "libmimedir.h"
#include "dirsynt.h"

mdir_line *
mdir_get_line(mdir_line **mdep, char *name) {

	if(!mdep || !name)
		return NULL;

	for(; *mdep; mdep++) {
		if(!strcasecmp((*mdep)->name, name))
			return *mdep;
	}

	return NULL;
}

char **
mdir_get_values_byname(mdir_line **mdep, char *name) {
	mdir_line *mdl;

	mdl = mdir_get_line(mdep, name);
	if(!mdl)
		return NULL;

	return mdl->values;
}

char *
mdir_get_value_byname(mdir_line **mdep, char *name) {
	mdir_line *mdl;

	mdl = mdir_get_line(mdep, name);
	if(!mdl)
		return NULL;

	if(!mdl->values)
		return NULL;

	return mdl->values[0];
}

char **
mdir_get_line_values(mdir_line *line) {

	if(!line)
		return NULL;

	return line->values;
}

char **
mdir_get_param_values(mdir_line *line, char *paramName) {
	mdir_param **vp;
	mdir_param *kvs;

	if(!line)
		return NULL;

	if(!line->params)
		return NULL;

	for(vp = line->params; *vp; vp++) {
		kvs = *vp;
		if(!strcasecmp(kvs->key, paramName))
			return kvs->vals;
	}

	return NULL;
}


