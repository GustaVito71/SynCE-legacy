/*-
 * Copyright (c) 2001, 2003, 2005 Lev Walkin <vlm@lionet.info>.
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
 * $Id: test.c,v 1.8 2005/03/03 20:47:46 vlm Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libmimedir.h"

char *malloc_options = "AJ";	/* Special debug for FreeBSD's allocator */

int
main(int ac, char **av) {
	mdir_line **mde;
	mdir_line **mdl;
	mdir_line *line;
	mdir_param *param;
	mdir_param **mdp;
	char **vp;
	char *test_file = "test.file";
	char *test_file_bad = "test.file.bad";

	setbuf(stdout, NULL);

	if(ac > 1)
		test_file = av[1];

	mde = mdir_parse_file(test_file_bad);
	if(mde) {
		fprintf(stderr, "Unexpected success\n");
		exit(1);
	}

	/*
	 * Suck the file in.
	 */
	mde = mdir_parse_file(test_file);
	if(!mde) {
		fprintf(stderr, "Can't parse\n");
		exit(1);
	}

	/*
	 * Concatenate with arbitrary data.
	 */
	mde = mdir_concat(mde, mdir_parse("some:thing"));

	/*
	 * Print out the card.
	 */
	for(mdl = mde; (line=*mdl); mdl++) {
		printf("%s", line->name);
		
		if(line->params) {
			for(mdp = line->params; (param=*mdp); mdp++) {
				printf(";");
				if(param->key)
					printf("%s=", param->key);
				if(param->vals) {
					for(vp = param->vals; *vp; vp++) {
						if(vp != param->vals)
							printf(",");
						printf("%s", *vp);
					}
				}
			}
		}

		if(line->values) {
			printf(":");
			for(vp = line->values; *vp; vp++) {
				if(vp != line->values)
					printf(",");
				printf("%s", *vp);
			}
		}

		printf("\n");
	}
	
	mdir_free(mde);

	return 0;
}
