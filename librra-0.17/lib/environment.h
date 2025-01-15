/* $Id: environment.h 1799 2004-08-17 16:13:21Z twogood $ */
#ifndef __environment_h__
#define __environment_h__

void* environment_push_timezone(const char* name);
void  environment_pop_timezone(void* handle);

#endif
