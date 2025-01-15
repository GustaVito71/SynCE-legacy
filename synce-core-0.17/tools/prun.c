/* $Id: prun.c 4136 2012-11-22 15:10:51Z mark_ellis $ */
#include "pcommon.h"
#include "rapi2.h"
#include <synce_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static void show_usage(const char* name)
{
	fprintf(stderr,
			"Syntax:\n"
			"\n"
			"\t%s [-d LEVEL] [-p DEVNAME] [-h] PROGRAM [--] [PARAMETERS]\n"
			"\n"
			"\t-d LEVEL    Set debug log level\n"
			"\t                0 - No logging (default)\n"
			"\t                1 - Errors only\n"
			"\t                2 - Errors and warnings\n"
			"\t                3 - Everything\n"
			"\t-h          Show this help message\n"
                        "\t-p DEVNAME  Mobile device name\n"
			"\tPROGRAM     The program you want to run\n"
      "\t--          Needed if PARAMETERS begin with a dash ('-')\n"
			"\tPARAMETERS  Parameters to the program\n",
			name);
}

static bool handle_parameters(int argc, char** argv, char** program, char** parameters, char** dev_name)
{
	int c;
	int log_level = SYNCE_LOG_LEVEL_LOWEST;

	while ((c = getopt(argc, argv, "d:p:h")) != -1)
	{
		switch (c)
		{
			case 'd':
				log_level = atoi(optarg);
				break;
			
                        case 'p':
                                *dev_name = optarg;
                                break;

			case 'h':
			default:
				show_usage(argv[0]);
				return false;
		}
	}

	synce_log_set_level(log_level);

	if (optind == argc)
	{
		fprintf(stderr, "%s: No program specified on command line\n\n", argv[0]);
		show_usage(argv[0]);
		return false;
	}
		
	*program = strdup(argv[optind++]);

	if (optind < argc)
		*parameters = strdup(argv[optind]);

	return true;
}

int main(int argc, char** argv)
{
	int result = 1;
	IRAPIDesktop *desktop = NULL;
	IRAPIEnumDevices *enumdev = NULL;
	IRAPIDevice *device = NULL;
	IRAPISession *session = NULL;
	RAPI_DEVICEINFO devinfo;
	char* program = NULL;
	char* parameters = NULL;
	HRESULT hr;
	WCHAR* wide_program = NULL;
	WCHAR* wide_parameters = NULL;
	PROCESS_INFORMATION info;
	char* dev_name = NULL;

	if (!handle_parameters(argc, argv, &program, &parameters, &dev_name))
		goto exit;

	if (FAILED(hr = IRAPIDesktop_Get(&desktop)))
	{
	  fprintf(stderr, "%s: failed to initialise RAPI: %d: %s\n", 
		  argv[0], hr, synce_strerror_from_hresult(hr));
	  goto exit;
	}

	if (FAILED(hr = IRAPIDesktop_EnumDevices(desktop, &enumdev)))
	{
	  fprintf(stderr, "%s: failed to get connected devices: %d: %s\n", 
		  argv[0], hr, synce_strerror_from_hresult(hr));
	  goto exit;
	}

	while (SUCCEEDED(hr = IRAPIEnumDevices_Next(enumdev, &device)))
	{
	  if (dev_name == NULL)
	    break;

	  if (FAILED(IRAPIDevice_GetDeviceInfo(device, &devinfo)))
	  {
	    fprintf(stderr, "%s: failure to get device info\n", argv[0]);
	    goto exit;
	  }
	  if (strcmp(dev_name, devinfo.bstrName) == 0)
	    break;
	}

	if (FAILED(hr))
	{
	  fprintf(stderr, "%s: Could not find device '%s': %08x: %s\n", 
		  argv[0],
		  dev_name?dev_name:"(Default)", hr, synce_strerror_from_hresult(hr));
	  device = NULL;
	  goto exit;
	}

	IRAPIDevice_AddRef(device);
	IRAPIEnumDevices_Release(enumdev);
	enumdev = NULL;

	if (FAILED(hr = IRAPIDevice_CreateSession(device, &session)))
	{
	  fprintf(stderr, "%s: Could not create a session to device: %08x: %s\n", 
		  argv[0], hr, synce_strerror_from_hresult(hr));
	  goto exit;
	}

	if (FAILED(hr = IRAPISession_CeRapiInit(session)))
	{
	  fprintf(stderr, "%s: Unable to initialize connection to device: %08x: %s\n", 
		  argv[0], hr, synce_strerror_from_hresult(hr));
	  goto exit;
	}


	convert_to_backward_slashes(program);
	wide_program = wstr_from_current(program);
        if (!wide_program) {
		fprintf(stderr, "%s: Failed to convert program '%s' from current encoding to UCS2\n", 
				argv[0],
				program);
		goto exit;
        }

	if (parameters) {
		wide_parameters = wstr_from_current(parameters);
                if (!wide_parameters) {
                        fprintf(stderr, "%s: Failed to convert parameters '%s' from current encoding to UCS2\n", 
				argv[0],
				program);
                        goto exit;
                }
        }

	memset(&info, 0, sizeof(info));
	
	if (!IRAPISession_CeCreateProcess(
				session,
				wide_program,
				wide_parameters,
				NULL,
				NULL,
				false,
				0,
				NULL,
				NULL,
				NULL,
				&info
				))
	{
		fprintf(stderr, "%s: Failed to execute '%s': %s\n", 
				argv[0],
				program,
				synce_strerror(IRAPISession_CeGetLastError(session)));
		goto exit;
	}

	IRAPISession_CeCloseHandle(session, info.hProcess);
	IRAPISession_CeCloseHandle(session, info.hThread);

	result = 0;

exit:
	wstr_free_string(wide_program);
	wstr_free_string(wide_parameters);

	if (program)
		free(program);

	if (parameters)
		free(parameters);

	if (session)
	{
	  IRAPISession_CeRapiUninit(session);
	  IRAPISession_Release(session);
	}

	if (device) IRAPIDevice_Release(device);
	if (enumdev) IRAPIEnumDevices_Release(enumdev);
	if (desktop) IRAPIDesktop_Release(desktop);
	return result;
}
