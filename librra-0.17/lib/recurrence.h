/* $Id: recurrence.h 4147 2012-11-26 15:15:54Z mark_ellis $ */
#ifndef __recurrence_h__
#define __recurrence_h__

#include <stdbool.h>
#include <libmimedir.h>
#include <rapitypes.h>
#include "mdir_line_vector.h"
#include "timezone.h"

struct _Generator;
struct _Parser;

bool recurrence_generate_rrule_vcal(
    struct _Generator* g, 
    CEPROPVAL* propval,
    RRA_Timezone *tzi);

bool recurrence_generate_rrule_ical(
    struct _Generator* g, 
    CEPROPVAL* propval,
    RRA_Timezone *tzi);

bool recurrence_parse_rrule(
    struct _Parser* p, 
    mdir_line* dtstart,
    mdir_line* dtend,
    mdir_line* rrule, 
    RRA_MdirLineVector* exdates,
    RRA_Timezone *tzi);

#endif

