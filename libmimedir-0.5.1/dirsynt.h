/* A Bison parser, made from dirsynt.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     C_PARAM_NAME = 258,
     C_PARAM_VALUE = 259,
     C_QUOTED_VALUE = 260,
     C_NAME = 261,
     C_VALUE = 262,
     C_EQ = 263,
     C_COMMA = 264,
     ERROR = 265
   };
#endif
#define C_PARAM_NAME 258
#define C_PARAM_VALUE 259
#define C_QUOTED_VALUE 260
#define C_NAME 261
#define C_VALUE 262
#define C_EQ 263
#define C_COMMA 264
#define ERROR 265




#ifndef YYSTYPE
#line 55 "dirsynt.y"
typedef union {
	char *tv_char;
	char **tv_list;
	mdir_param *tv_mdir_param;
	mdir_param **tv_mdir_param_list;
	mdir_line *tv_mdir_line;
	mdir_line **tv_mdir_line_list;
} yystype;
/* Line 1281 of /usr/local/share/bison/yacc.c.  */
#line 69 "y.tab.h"
# define YYSTYPE yystype
#endif




#endif /* not BISON_Y_TAB_H */

