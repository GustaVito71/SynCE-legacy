/* $Id: strv.h 1624 2004-04-28 14:58:31Z twogood $ */
#ifndef __strv_h__
#define __strv_h__

char** strsplit(const char* source, int separator);
void strv_dump(char** strv);
void strv_free(char** strv);

#endif
