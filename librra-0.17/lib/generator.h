/* $Id: generator.h 4147 2012-11-26 15:15:54Z mark_ellis $ */
#ifndef __generator_h__
#define __generator_h__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> /* for size_t */
#include <rapitypes.h>
#include "../rra_config.h"

#define GENERATOR_UTF8 1

typedef struct _GeneratorProperty GeneratorProperty;
typedef struct _Generator Generator;

typedef struct _GeneratorParam
{
  char* name;
  char** values;
} GeneratorParam;


typedef bool (*GeneratorPropertyFunc)(Generator* g, CEPROPVAL* property, void* cookie);

Generator* generator_new(int flags, void* cookie);
void generator_destroy(Generator* self);
bool generator_utf8(Generator* self);

void generator_add_property(Generator* self, uint16_t id, GeneratorPropertyFunc func);

bool generator_set_data(Generator* self, const uint8_t* data, size_t data_size);
bool generator_run(Generator* self);
bool generator_get_result(Generator* self, char** result);

bool generator_add_simple(Generator* self, const char* name, const char* value);
bool generator_add_simple_unescaped(Generator* self, const char* name, const char* value);
bool generator_add_with_type(Generator* self, const char* name, const char* type, const char* value);

bool generator_add_simple_propval(Generator* self, const char* name, CEPROPVAL* propval);

/* for multi-parameter or multi-value lines */
bool generator_begin_line(Generator* self, const char* name);
bool generator_begin_parameter(Generator* self, const char* name);
bool generator_add_parameter_value(Generator* self, const char* value);
bool generator_end_parameter(Generator* self);
bool generator_add_value(Generator* self, const char* value);
bool generator_end_line(Generator* self);

#endif

