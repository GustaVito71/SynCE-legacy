/* $Id: rra-timezone.c 4151 2012-12-31 19:12:38Z mark_ellis $ */
#define _POSIX_C_SOURCE 2 /* for getopt */
#define _BSD_SOURCE
#include "../lib/timezone.h"
#include "../lib/generator.h"
#include <rapi2.h>
#include <synce_log.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum 
{
  FORMAT_UNKNOWN,
  FORMAT_BINARY,
  FORMAT_RAW,
  FORMAT_VTIMEZONE
} OutputFormat;

static void show_usage(const char* name)/*{{{*/
{
  fprintf(stderr,
      "Syntax:\n"
      "\n"
      "\t%s [-d LEVEL] [-h] [-i INPUT-FILE] [-o OUTPUT-FILE] [-r] [-v]\n"
      "\n"
      "\t-d LEVEL           Set debug log level\n"
      "\t                     0 - No logging (default)\n"
      "\t                     1 - Errors only\n"
      "\t                     2 - Errors and warnings\n"
      "\t                     3 - Everything\n"
      "\t                   Binary output format\n"
      "\t-h                 Show this help message\n"
      "\t-i INPUT-FILE      Read binary time zone information from this file.\n"
      "\t                   Time zone information is retrieved from PDA if this\n"
      "\t                   parameter is left out.\n"
      "\t-o OUTPUT-FILE     Write binary time zone information to this file.\n"
      "\t                   Standard output is used if this parameter is missing.\n"
      "\t-b                 binary format\n"
      "\t-r                 Raw format\n"
      "\t-v                 VTIMEZONE output format (Default)\n"
      ,
      name);
}/*}}}*/

static bool handle_parameters(int argc, char** argv, /*{{{*/
    char** input, char** output, OutputFormat* format)
{
  int c;
  int log_level = SYNCE_LOG_LEVEL_LOWEST;

  if (!input || !output)
    return false;
  
  *input = NULL;
  *output = NULL;

  *format = FORMAT_VTIMEZONE;

  while ((c = getopt(argc, argv, "bd:hi:o:rv")) != -1)
  {
    switch (c)
    {
      case 'b':
        *format = FORMAT_BINARY;
        break;

      case 'd':
        log_level = atoi(optarg);
        break;

      case 'i': 
        if (*input)
          free(*input);
        *input = strdup(optarg);
        break;

      case 'o': 
        if (*output)
          free(*output);
        *output = strdup(optarg);
        break;

      case 'r':
        *format = FORMAT_RAW;
        break;

      case 'v':
        *format = FORMAT_VTIMEZONE;
        break;
            
      case 'h':
      default:
        show_usage(argv[0]);
        return false;
    }
  }

  synce_log_set_level(log_level);

  return true;
}/*}}}*/

const char* month_names[12] =/*{{{*/
{
  "January",
  "Februrary",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};/*}}}*/

static const char* month_string(unsigned month)/*{{{*/
{
  if (month >= 1 && month <= 12)
    return month_names[month-1];
  else
    return "Unknown";
}/*}}}*/

static const char* instance_string(unsigned instance)/*{{{*/
{
  switch (instance)
  {
    case 1: return "First week of month";
    case 2: return "Second week of month";
    case 3: return "Third week of month";
    case 4: return "Fourth week of month";
    case 5: return "Last week of month";
    default: return "Unknown";
  }
}/*}}}*/

void dump_vtimezone(FILE* output, RRA_Timezone* tzi)/*{{{*/
{
  Generator* generator = generator_new(0, NULL);
  
  if (rra_timezone_generate_vtimezone(generator, tzi))
  {
    char* result = NULL;
    if (generator_get_result(generator, &result))
    {
      fprintf(output, "%s", result);
      free(result);
    }
    else
      fprintf(stderr, "Failed to get generated VTIMEZONE\n");
  }
  else
    fprintf(stderr, "Failed to generate VTIMEZONE\n");

  generator_destroy(generator);
}/*}}}*/

static bool get_rra_timezone_information(const char* argv0, RRA_Timezone* tzi, char* input)/*{{{*/
{
  bool success = false;
  IRAPIDesktop *desktop = NULL;
  IRAPIEnumDevices *enumdev = NULL;
  IRAPIDevice *device = NULL;
  IRAPISession *session = NULL;
  HRESULT hr;

  if (input)
  {
    FILE* file = fopen(input, "r");

    if (file)
    {
      size_t bytes_read = fread(tzi, 1, sizeof(RRA_Timezone), file);
      if (sizeof(RRA_Timezone) == bytes_read)
      {
        success = true;
      }
      else
      {
        fprintf(stderr, "%s: Only read %i bytes from time zone information file '%s': %s\n", 
            argv0, (int) bytes_read, input, strerror(errno));
      }

      fclose(file);
    }
    else
    {
      fprintf(stderr, "%s: Unable to open time zone information file '%s': %s\n", 
          argv0, input, strerror(errno));
    }
    return success;
  }




  if (FAILED(hr = IRAPIDesktop_Get(&desktop)))
  {
    fprintf(stderr, "%s: failed to initialise RAPI: %d: %s\n", 
            argv0, hr, synce_strerror_from_hresult(hr));
    goto exit;
  }

  if (FAILED(hr = IRAPIDesktop_EnumDevices(desktop, &enumdev)))
  {
    fprintf(stderr, "%s: failed to get connected devices: %d: %s\n", 
        argv0, hr, synce_strerror_from_hresult(hr));
    goto exit;
  }

  if (FAILED(hr = IRAPIEnumDevices_Next(enumdev, &device)))
  {
    fprintf(stderr, "%s: Could not find device: %08x: %s\n", 
        argv0, hr, synce_strerror_from_hresult(hr));
    device = NULL;
    goto exit;
  }

  IRAPIDevice_AddRef(device);
  IRAPIEnumDevices_Release(enumdev);
  enumdev = NULL;

  if (FAILED(hr = IRAPIDevice_CreateSession(device, &session)))
  {
    fprintf(stderr, "%s: Could not create a session to device: %08x: %s\n", 
        argv0, hr, synce_strerror_from_hresult(hr));
    goto exit;
  }

  if (FAILED(hr = IRAPISession_CeRapiInit(session)))
  {
    fprintf(stderr, "%s: Unable to initialize connection to device: %08x: %s\n", 
        argv0, hr, synce_strerror_from_hresult(hr));
    goto exit;
  }

  if (rra_timezone_get(tzi, session))
    success = true;
  else
    fprintf(stderr, "%s: Failed to get time zone information\n", argv0);

exit:
  if (session)
  {
    IRAPISession_CeRapiUninit(session);
    IRAPISession_Release(session);
  }

  if (device) IRAPIDevice_Release(device);
  if (enumdev) IRAPIEnumDevices_Release(enumdev);
  if (desktop) IRAPIDesktop_Release(desktop);

  return success;
}/*}}}*/

int main(int argc, char** argv)
{
  int result = 1;
  RRA_Timezone tzi;
  char* input_filename;
  char* output_filename;
  OutputFormat format;
  FILE* output = stdout;

  if (!handle_parameters(argc, argv, &input_filename, &output_filename, &format))
    goto exit;

  if (!get_rra_timezone_information(argv[0], &tzi, input_filename))
    goto exit;

  if (output_filename)
  {
    output = fopen(output_filename, "w");

    if (!output)
    {
      fprintf(stderr, "%s: Failed to open file %s: %s\n", 
          argv[0], output_filename, strerror(errno));
      goto exit;
    }
  }

  switch (format)
  {
    case FORMAT_BINARY:
      if (stdout == output)
      {
        fprintf(stderr, "%s: Refusing to use binary format on standard output.\n", 
            argv[0]);
        goto exit;
      }
      
      if (fwrite(&tzi, sizeof(RRA_Timezone), 1, output) != 1)
      {
        fprintf(stderr, "%s: Failed to write data to file %s: %s\n", 
            argv[0], output_filename, strerror(errno));
        goto exit;
      }
      break;

    case FORMAT_RAW:
      {
        char* standard_name = wstr_to_current(tzi.StandardName);
        if (!standard_name) {
                fprintf(stderr, "Failed to convert StandardName to current encoding");
                standard_name = strdup("");
        }
        char* daylight_name = wstr_to_current(tzi.DaylightName);
        if (!daylight_name) {
                fprintf(stderr, "Failed to convert DaylightName to current encoding");
                daylight_name = strdup("");
        }

        fprintf(output,
            "Bias:                  %i\n"
            "StandardName:          %s\n"
            "StandardMonthOfYear:   %i (%s)\n"
            "StandardInstance:      %i (%s)\n"
            "StandardStartHour:     %i\n"
            "StandardBias:          %i\n"
            "DaylightName:          %s\n"
            "DaylightMonthOfYear:   %i (%s)\n"
            "DaylightInstance:      %i (%s)\n"
            "DaylightStartHour:     %i\n"
            "DaylightBias:          %i\n"
            ,
            tzi.Bias,
            standard_name,
            tzi.StandardMonthOfYear, month_string(tzi.StandardMonthOfYear),
            tzi.StandardInstance, instance_string(tzi.StandardInstance),
            tzi.StandardStartHour,
            tzi.StandardBias,
            daylight_name,
            tzi.DaylightMonthOfYear, month_string(tzi.DaylightMonthOfYear),
            tzi.DaylightInstance, instance_string(tzi.DaylightInstance),
            tzi.DaylightStartHour,
            tzi.DaylightBias
              );

            wstr_free_string(standard_name);
            wstr_free_string(daylight_name);
      }
      break;

    case FORMAT_VTIMEZONE:
    default:
      dump_vtimezone(output, &tzi);
      break;
  }

  result = 0;

exit:
  if (output && output != stdout)
    fclose(output);
  if (output_filename)
    free(output_filename);
  if (input_filename)
    free(input_filename);
  return result;
}

