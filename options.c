#include "options.h"

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

programmode_t options_mode = MODE_DEFAULT;
char *options_test_param;

time_t options_destination_time;
time_t options_present_time;
time_t options_departed_time;

int options_destination_time_valid = 0;
int options_present_time_valid = 0;
int options_departed_time_valid = 0;

int options_utc = 1;
int options_relative = 0;
int options_hour24 = 0;
int options_seconds = 0;
int options_loop = 0;

static void print_usage(const char *prog)
{
	printf("Usage: %s [options]\n"
		   "  -t, --test=<MODE>           run test mode <MODE>. Don't supply a mode to show a list of all supported modes.\n"
		   "  -d, --destination=<TIME>    set destination time to <TIME>\n"
		   "  -p, --present=<TIME>        set present time to <TIME>\n"
		   "  -D, --departed=<TIME>       set last departed time to <TIME>\n"
		   "  -u, --utc                   interpret all times as UTC\n"
		   "  -l, --local                 interpret all times in local time\n"
		   "  -r, --relative              interpret all times as relative\n"
		   "  -H, --hour24=[0|1]          display in 24 hour mode\n"
		   "  -s, --seconds=[0|1]         display seconds\n"
		   "  -L, --loop                  loop\n"
			, prog);
	exit(1);
}

static const struct option lopts[] = {
	{"test", optional_argument , 0, 't'},
	{"destination", required_argument, 0, 'd'},
	{"present", required_argument, 0, 'p'},
	{"departed", required_argument, 0, 'D'},
	{"utc", no_argument, 0, 'u'},
	{"local", no_argument, 0, 'l'},
	{"relative", no_argument, 0, 'r'},
	{"hour24", required_argument, 0, 'H'},
	{"seconds", required_argument, 0, 's'},
	{"loop", no_argument, 0, 'L'},
	{ NULL, 0, 0, 0 },
};

static const char sopts[] = "t:d:p:D:ulrH:s:L"; //D:s:d:b:lHOLC3NR

void options_parse(int argc, char *argv[])
{
	while (1) {
		int c;

		c = getopt_long(argc, argv, sopts, lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 't':
			options_mode = MODE_TEST;
			options_test_param = optarg;
			break;

		case 'd':
			options_destination_time = strtoll(optarg, NULL, 0);
			options_destination_time_valid = 1;
			break;

		case 'p':
			options_present_time = strtoll(optarg, NULL, 0);
			options_present_time_valid = 1;
			break;

		case 'D':
			options_departed_time = strtoll(optarg, NULL, 0);
			options_departed_time_valid = 1;
			break;

		case 'u':
			options_utc = 1;
			break;

		case 'l':
			options_utc = 0;
			break;

		case 'r':
			options_relative = 1;
			break;

		case 'H':
			options_hour24 = !(strtol(optarg, NULL, 0) == 0);
			break;

		case 's':
			options_seconds = !(strtol(optarg, NULL, 0) == 0);
			break;

		case 'L':
			options_loop = 1;
			break;

		default:
			print_usage(argv[0]);
			break;
		}
	}
}
