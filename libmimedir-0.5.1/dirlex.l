%{
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
 * $Id: dirlex.l,v 1.8 2005/03/03 20:48:51 vlm Exp $
 */

#include "intern.h"
#include "dirsynt.h"

#define	YY_DECL	int yylex (YYSTYPE *_mdir_lval, void *arg)

%}

%option never-interactive
%option noyywrap 8bit stack
%option noinput nounput

%pointer

	/* Basic character sets */

WSP	[ \t]
DQUOTE	\x22

NON-ASCII	[\x80-\xff]
QSAFE-CHAR	[ \t\x21\x23-\x7e\x80-\xff]
	/* Any character except CTLs, DQUOTE */

SAFE-CHAR	[ \t\x21\x23-\x2b\x2d-\x39\x3c-\x7e\x80-\xff]
	/* Any character except CTLs, DQUOTE, ";", ":", "," */

VALUE-CHAR	[ \t\x21-\x7e\x80-\xff]
	/* WSP + visible (printing) characters + NON-ASCII */

VALUE-CHAR-NOBS	[ \t\x21-\x5b\x5d-\x7e\x80-\xff]
	/* WSP + visible (printing) characters + NON-ASCII without backslash */
 
	/* Basic tokens */

GROUP	[[:alpha:][:digit:]-]+
NAME	[[:alpha:][:digit:]-]+

VALUE	{VALUE-CHAR}*	/* + 5.8.4 */
PTEXT	{SAFE-CHAR}*
QUOTED-STRING	{DQUOTE}{QSAFE-CHAR}*{DQUOTE}

PARAM-VALUE	{PTEXT}|{QUOTED-STRING}

PARAM	{NAME}"="{PARAM-VALUE}

CONTENTLINE	{NAME}(";"{PARAM})*":"{VALUE-CHAR}*\n

%x optParam
%x Value
%x TextValue
%x paramName
%x paramValue
%x paramQuotedValue
%%

({GROUP}".")?{NAME}	{
		yy_push_state(optParam);
		_mdir_lval->tv_char = _mdir_mem_strdup(arg, yytext);
		return C_NAME;
	};

<optParam>{
	";"	yy_push_state(paramName);
	":"		{
		yy_pop_state();
		yy_push_state(TextValue);
		// yy_push_state(Value);
	}
}

<paramName>{
	[\r]?\n[ \t]	/* Just ignore it */
	{NAME}	{
			_mdir_lval->tv_char = _mdir_mem_strdup(arg, yytext);
			return C_PARAM_NAME;
		}
	"="	{
			yy_pop_state();
			yy_push_state(paramValue);
			return C_EQ;
		}
}

<paramValue>{
	({PTEXT}|{QUOTED-STRING})[,]?	{
			char *p = yytext;
			int plen = yyleng;

			/* Crop the tailing comma */
			if(p[plen - 1] == ',')
				p[--plen] = 0;
			else {
				yy_pop_state();
			}

			/* Crop the quotes */
			if(*p == '"') {
				p[--plen] = 0;	/* Crop quote */
				p += 1;	/* Skip quote */
			}

			_mdir_lval->tv_char = _mdir_mem_strdup(arg, p);
			return C_PARAM_VALUE;
		}

	{DQUOTE}	yy_push_state(paramQuotedValue);

	[:;]	{
			yy_pop_state();
			yyless(0);
			_mdir_lval->tv_char = _mdir_mem_strdup(arg, "");
			return C_PARAM_VALUE;
	}
}

<paramQuotedValue>{
	[\r]?\n[ \t]	/* Just ignore it */

	{DQUOTE}	yy_pop_state();

	{QSAFE-CHAR}*	{
		_mdir_lval->tv_char = yytext;
		return C_QUOTED_VALUE;
	}
}


<Value>{
	{VALUE-CHAR}*	{
		_mdir_lval->tv_char = yytext;
		return C_VALUE;
	}
	[\r]?\n[ \t]	/* Just ignore it */
	[\r]?\n	{
			yy_pop_state();
			_mdir_lval->tv_char = yytext;
			return C_VALUE;
		}
}

<TextValue>{
	"\\"[nN]	{
		_mdir_lval->tv_char = "\n";
		return C_VALUE;
		}
	"\\".	{
		_mdir_lval->tv_char = yytext + 1;
		return C_VALUE;
	}

	{VALUE-CHAR-NOBS}*	{
		_mdir_lval->tv_char = yytext;
		return C_VALUE;
	}

	[\r]?\n[ \t]	/* Just ignore it */
	[\r]?\n	{
			yy_pop_state();
			_mdir_lval->tv_char = "";
			return C_VALUE;
		}
}

[\r]?\n		{ return 0; }	/* Finish */

<*>.|\n	{
		while(YY_START)
			yy_pop_state();
		yy_top_state();
		return ERROR;
	}

<*><<EOF>>	{
		while(YY_START)
			yy_pop_state();
		yyterminate();
	}

%%
