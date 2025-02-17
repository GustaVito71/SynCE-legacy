/* $Id: task.h 4111 2012-10-18 15:29:41Z mark_ellis $ */
#ifndef __task_h__
#define __task_h__

#include <synce.h>

struct _RRA_Timezone;

#define RRA_TASK_ID_UNKNOWN  0

#define RRA_TASK_NEW     		0x1
#define RRA_TASK_UPDATE  		0x2
#define RRA_TASK_COMMAND_MASK		0xf

#define RRA_TASK_ISO8859_1		0x10
#define RRA_TASK_UTF8			0x20
#define RRA_TASK_CHARSET_MASK		0xf0

#define RRA_TASK_VCAL_1_0		0x100
#define RRA_TASK_VCAL_2_0		0x200
#define RRA_TASK_VERSION_MASK		0xf00
#define RRA_TASK_VERSION_DEFAULT	RRA_TASK_VCAL_1_0

#ifndef SWIG
bool rra_task_to_vtodo(
    uint32_t id,
    const uint8_t* data,
    size_t data_size,
    char** vtodo,
    uint32_t flags,
    struct _RRA_Timezone* tzi,
    const char *codepage);

bool rra_task_from_vtodo(
    const char* vtodo,
    uint32_t* id,
    uint8_t** data,
    size_t* data_size,
    uint32_t flags,
    struct _RRA_Timezone* tzi,
    const char *codepage);

#define rra_task_free_vtodo(p)  if (p) free(p)
#define rra_task_free_data(p)   if (p) free(p)

#endif /* SWIG */

#endif
