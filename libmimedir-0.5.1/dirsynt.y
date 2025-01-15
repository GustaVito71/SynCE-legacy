%{
/*-
 * Copyright (c) 2001, 2003 Lev Walkin <vlm@spelio.net.ru>.
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
 * $Id: dirsynt.y,v 1.5 2003/07/30 05:22:47 vlm Exp $
 */

#include "libmimedir.h"
#include "intern.h"

#undef	YYPURE
#define	YYPURE	1

#define	YYPARSE_PARAM	arg
#define	YYLEX_PARAM	arg

#define _mdir_error(foo)	_mdir_mem_forget(arg, 1)

int yylex();

/*
 * Length of the token.
 */
//extern int _mdir_leng;

int _mdir_parse_error = 0;

%}

%pure-parser

/* Basic union */
%union	{
	char *tv_char;
	char **tv_list;
	mdir_param *tv_mdir_param;
	mdir_param **tv_mdir_param_list;
	mdir_line *tv_mdir_line;
	mdir_line **tv_mdir_line_list;
}

/* Complex tokens returned by parser */
%token	<tv_char>	C_PARAM_NAME
%token	<tv_char>	C_PARAM_VALUE
%token	<tv_char>	C_QUOTED_VALUE
%token	<tv_char>	C_NAME
%token	<tv_char>	C_VALUE

/* Simple tokens returned by parser */
%token	C_EQ
%token	C_COMMA
%token	ERROR

%type	<tv_mdir_param_list>	Params;
%type	<tv_mdir_param>	Param_def;
%type	<tv_list>	ParamValueList;
%type	<tv_char>	ParamValue;
%type	<tv_char>	ParamQuotedValue;
%type	<tv_list>	ValueList;
%type	<tv_char>	Value;

%type	<tv_mdir_line>	CONTENTLINE;
%type	<tv_mdir_line_list>	MIME_DIR_ENTRY;

%%

MIME_DIR:
	MIME_DIR_ENTRY {
		if(_mdir_parse_error) {
			_mdir_mem_forget(arg, 1);
			*(mdir_line ***)arg = NULL;
			return -1;
		}
		_mdir_mem_forget(arg, 0);
		*(mdir_line ***)arg = $1;
	}
	;

MIME_DIR_ENTRY:
	CONTENTLINE {
		$$ = (mdir_line **)_mdir_mem_list_extend(arg, NULL);
		if($$) {
			*$$ = (mdir_line *)$1;
		} else {
			_mdir_parse_error = 1;
		}
	}
	| MIME_DIR_ENTRY CONTENTLINE {
		mdir_line **p = (mdir_line **)_mdir_mem_list_extend(arg, (char ***)&($1));
		if(p) {
			*p = $2;
			$$ = $1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

CONTENTLINE:
	C_NAME ValueList {
		$$ = (mdir_line *)_mdir_mem_malloc(arg, sizeof(mdir_line));
		if($$) {
			$$->name = $1;
			$$->params = NULL;
			$$->values = $2;
		} else {
			_mdir_parse_error = 1;
		}
	}
	| C_NAME Params ValueList {
		$$ = (mdir_line *)_mdir_mem_malloc(arg, sizeof(mdir_line));
		if($$) {
			$$->name = $1;
			$$->params = $2;
			$$->values = $3;
		} else {
			_mdir_parse_error = 1;
		}
	}
	;

Params:
	Param_def {
		$$ = (mdir_param **)_mdir_mem_list_extend(arg, NULL);
		if($$) {
			*$$ = $1;
		} else {
			_mdir_parse_error = 1;
		}
	}
	| Params Param_def {
		mdir_param **p = (mdir_param **)_mdir_mem_list_extend(arg, (char ***)&($1));
		if(p) {
			*p = $2;
			$$ = $1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

Param_def:
	C_PARAM_NAME C_EQ ParamValueList {
		$$ = (mdir_param *)_mdir_mem_malloc(arg, sizeof(mdir_param));
		if($$) {
			$$->key = $1;
			$$->vals = $3;
		} else {
			_mdir_parse_error = 1;
		}
	}
	;

ParamValueList:
	ParamValue {
		$$ = _mdir_mem_list_extend(arg, NULL);
		if($$) {
			*($$) = $1;
		} else {
			_mdir_parse_error = 1;
		}
	}
	| ParamValueList ParamValue {
		char **p = _mdir_mem_list_extend(arg, &($1));
		if(p) {
			*p = $2;
			$$ = $1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

ValueList:
	Value {
		$$ = _mdir_mem_list_extend(arg, NULL);
		if($$) {
			*($$) = $1;
		} else {
			_mdir_parse_error = 1;
		}
	}
	| ValueList C_COMMA Value {
		char **p = _mdir_mem_list_extend(arg, &($1));
		if(p) {
			*p = $3;
			$$ = $1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

ParamValue:
	C_PARAM_VALUE {
		$$ = _mdir_mem_strdup(arg, $1);
		if(!($$))
			_mdir_parse_error = 1;
	}
	| ParamQuotedValue {
		$$ = $1;
	}
	;

ParamQuotedValue:
	C_QUOTED_VALUE {
		$$ = _mdir_mem_strdup(arg, $1);
		if(!($$))
			_mdir_parse_error = 1;
	}
	| ParamQuotedValue C_QUOTED_VALUE {
		if($1) {
			$$ = _mdir_mem_stradd(arg, $1, $2);
			if(!($$))
				_mdir_parse_error = 1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

Value:
	C_VALUE {
		$$ = _mdir_mem_strdup(arg, $1);
		if(!($$))
			_mdir_parse_error = 1;
	}
	| Value C_VALUE {
		if($1) {
			$$ = _mdir_mem_stradd(arg, $1, $2);
			if(!($$))
				_mdir_parse_error = 1;
		} else {
			_mdir_parse_error = 1;
			$$ = NULL;
		}
	}
	;

%%
