/* $Id: rra-task-to-vtodo.c 3750 2009-04-14 12:43:36Z mark_ellis $ */
#include "../lib/task.h"
#include "../lib/timezone.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <synce_log.h>

static void show_usage(const char* name)
{
	fprintf(stderr,
			"Syntax:\n"
			"\n"
			"\t%s [-d LEVEL] [-c CODEPAGE] TASK_FILE [VTODO_FILE]\n"
			"\n"
			"\t-d LEVEL          Set debug log level\n"
			"\t                  0 - No logging\n"
			"\t                  1 - Errors only (default)\n"
			"\t                  2 - Errors and warnings\n"
			"\t                  3 - Errors, warnings and info\n"
			"\t                  4 - Everything\n"
                        "\t-c CODEPAGE       Codepage to be used for TASK_FILE (default CP1252)\n"
                        "\t-u                Output vTodo as UTF8\n"
                        "\t-t TZFILE         Timezone filename\n"
			"\tTASK_FILE         The source task filename\n"
			"\tVTODO_FILE        The destination vtodo filename\n",
			name);
}

static bool handle_parameters(int argc, char** argv, char** source, char** dest, char** tzfile, char** codepage, uint32_t *flags)
{
	int c;
	int path_count;
	int log_level = SYNCE_LOG_LEVEL_ERROR;

	while ((c = getopt(argc, argv, "d:c:t:u")) != -1)
	{
		switch (c)
		{
			case 'd':
				log_level = atoi(optarg);
				break;
			
			case 'c':
				*codepage = optarg;
				break;

			case 'u':
				*flags = *flags | RRA_TASK_UTF8;
				break;

			case 't':
				*tzfile = optarg;
				break;

			case 'h':
			default:
				show_usage(argv[0]);
				return false;
		}
	}
				
	synce_log_set_level(log_level);

	path_count = argc - optind;
	if (path_count < 1 || path_count > 2) {
		fprintf(stderr, "%s: You need to specify source and optional destination file names on command line\n\n", argv[0]);
		show_usage(argv[0]);
		return false;
	}
		
	*source = strdup(argv[optind++]);
	if (path_count > 1)
	       	*dest = strdup(argv[optind++]);

	return true;
}


int main(int argc, char** argv)
{
	int result = 1;
	FILE* file = NULL;
	uint8_t* buffer = NULL;
	long file_size = 0;
	char* vtodo = NULL;
	RRA_Timezone tzi;
	RRA_Timezone* p_tzi = NULL;
	char *source = NULL, *dest = NULL, *tzfile = NULL;
	char *codepage = NULL;
	uint32_t flags = 0;

	if (!handle_parameters(argc, argv, &source, &dest, &tzfile, &codepage, &flags))
		goto exit;

	if (!codepage)
		codepage = "CP1252";

	file = fopen(source, "r");
	if (!file)
	{
		fprintf(stderr, "Unable to open file '%s'\n", source);
		goto exit;
	}

	/* find out file size */
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (uint8_t*)malloc(file_size);
	if (fread(buffer, file_size, 1, file) != 1)
	{
		fprintf(stderr, "Unable to read data from file '%s'\n", source);
		goto exit;
	}
        fclose(file);

	if (tzfile)
	{
		FILE* file = fopen(tzfile, "r");
		if (file)
		{
			size_t bytes_read = fread(&tzi, 1, sizeof(RRA_Timezone), file);
			if (sizeof(RRA_Timezone) == bytes_read)
			{
				p_tzi = &tzi;
			}
			else
			{
				fprintf(stderr, "%s: Only read %i bytes from time zone information file '%s': %s\n", 
					argv[0], (int) bytes_read, tzfile, strerror(errno));
				goto exit;
			}

			fclose(file);
		}
		else
		{
			fprintf(stderr, "%s: Unable to open time zone information file '%s': %s\n", 
				argv[0], tzfile, strerror(errno));
			goto exit;
		}
	}

	if (!rra_task_to_vtodo(
			RRA_TASK_ID_UNKNOWN,
			buffer,
			file_size,
			&vtodo,
			flags,
			p_tzi,
			codepage))
	{
		fprintf(stderr, "Failed to create vEvent\n");
		goto exit;
	}
	
	if (dest) {
		file = fopen(dest, "w");
		if (!file)
		{
			fprintf(stderr, "Unable to open file '%s'\n", dest);
			goto exit;
		}
		fprintf(file, "%s", vtodo);
	} else {
		printf("%s", vtodo);
	}

	result = 0;

exit:
	if (file)
		fclose(file);
	
	if (buffer)
		free(buffer);	

	if (vtodo)
		free(vtodo);
	
	return result;
}
