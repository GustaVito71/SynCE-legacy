/* $Id: rra-delete.c 4151 2012-12-31 19:12:38Z mark_ellis $ */
#define _BSD_SOURCE 1
#include "../lib/syncmgr.h"
#include <rapi2.h>
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
			"\t%s [-d LEVEL] TYPE-ID OBJECT-ID ...\n"
			"\n"
			"\t-d LEVEL          Set debug log level\n"
			"\t                  0 - No logging\n"
			"\t                  1 - Errors only (default)\n"
			"\t                  2 - Errors and warnings\n"
			"\t                  3 - Errors, warnings and info\n"
			"\t                  4 - Everything\n"
			"\tTYPE-ID           RRA type-id of the object to delete\n"
			"\tOBJECT-ID ...     One or more RRA object-id's of the objects to delete\n",
			name);
}

static bool handle_parameters(int argc, char** argv, char** typeid, char*** object_id_list, uint *object_id_no)
{
	int c;
	int arg_count;
	int log_level = SYNCE_LOG_LEVEL_ERROR;

	while ((c = getopt(argc, argv, "d:")) != -1)
	{
		switch (c)
		{
			case 'd':
				log_level = atoi(optarg);
				break;
			
			case 'h':
			default:
				show_usage(argv[0]);
				return false;
		}
	}

	synce_log_set_level(log_level);

	arg_count = argc - optind;
	if (arg_count < 2) {
		fprintf(stderr, "%s: You need to specify type id and object id on command line\n\n", argv[0]);
		show_usage(argv[0]);
		return false;
	}

	*typeid = strdup(argv[optind++]);

        *object_id_no = arg_count - 1;
       	*object_id_list = &argv[optind];

	return true;
}

int main(int argc, char** argv)
{
	int result = 1;
	IRAPIDesktop *desktop = NULL;
	IRAPIEnumDevices *enumdev = NULL;
	IRAPIDevice *device = NULL;
	IRAPISession *session = NULL;
	HRESULT hr;
	RRA_SyncMgr* syncmgr = NULL;
	char* type_id_str = NULL;
	uint32_t type_id = 0;
	uint32_t object_id = 0;
	const RRA_SyncMgrType* type = NULL;
	int i;
	char** object_id_list = NULL;
	uint object_id_no = 0;

	if (!handle_parameters(argc, argv, &type_id_str, &object_id_list, &object_id_no))
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

        if (FAILED(hr = IRAPIEnumDevices_Next(enumdev, &device)))
        {
          fprintf(stderr, "%s: Could not find device: %08x: %s\n", 
                  argv[0], hr, synce_strerror_from_hresult(hr));
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

	syncmgr = rra_syncmgr_new();

	if (!rra_syncmgr_connect(syncmgr, session))
	{
		fprintf(stderr, "Connection failed\n");
		goto exit;
	}

  type = rra_syncmgr_type_from_name(syncmgr, type_id_str);
  if (type)
    type_id = type->id;
  else
    type_id = strtol(type_id_str, NULL, 16);

  for (i = 0; i < object_id_no; i++) 
  {
    object_id = strtol(object_id_list[i], NULL, 16);

    if (!rra_syncmgr_delete_object(syncmgr, type_id, object_id))
    {
      fprintf(stderr, "Failed to delete object %08x\n", object_id);
      goto exit;
    }
  }

	result = 0;
	
exit:
	rra_syncmgr_destroy(syncmgr);
	
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
