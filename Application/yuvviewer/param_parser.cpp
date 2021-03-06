#define _CRT_SECURE_NO_WARNINGS

#include <zstack.h>

#include <3rdparty/getopt.h>

#include "main.h"

enum {
    OPTION_VERSION = 1,
    OPTION_HELP,
    OPTION_LOG,

	OPTION_RESOLUTION,
	OPTION_STRIDE,
	OPTION_FORMAT
};

static struct option opts[] = {
    {"version", no_argument, 0, OPTION_VERSION},
    {"help", no_argument, 0, OPTION_HELP},
    {"log", required_argument, 0, OPTION_LOG},
	{"format", required_argument, 0, OPTION_FORMAT},
	{"resolution", required_argument, 0, OPTION_RESOLUTION},
	{"stride", required_argument, 0, OPTION_STRIDE},
    {0, 0, 0, 0}
};

int param_parser(int argc, char *argv[], struct application *app)
{
    int c;

    while((c=getopt_long(argc, argv, "", opts, NULL))!=-1){
        switch (c) {
        case OPTION_VERSION:
            log(LOG_USER, "Version: %s\n", version);
            break;
        case OPTION_HELP:
            log(LOG_USER, usage);
            break;
        case OPTION_LOG:
            if (strlen(optarg) >= LOG_CONFIG_LENGTH) {
                log(LOG_USER, "log config out of range (0, %d)\n", LOG_CONFIG_LENGTH);
                return -1;
            }
            strncpy(app->param.log_config, optarg, LOG_CONFIG_LENGTH);
            break;
		case OPTION_FORMAT:
			if (0 == strcmp("I420", optarg))
				app->param.format = YUV_I420;
			else
				return -1;
			break;
		case OPTION_RESOLUTION:
			sscanf(optarg, "%dx%d", &app->param.width, &app->param.height);
			break;
		case OPTION_STRIDE:
			app->param.stride = atoi(optarg);
			break;
        default:
            return -1;
        }
    }

    if (argc > 1) {
        strncpy(app->param.filename, optarg, FILENAME_MAX - 1);
    }

    // Do param validation

    return 0;
}
