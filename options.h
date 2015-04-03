#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <time.h>

typedef enum {
	MODE_DEFAULT,
	MODE_TEST
} programmode_t;

void options_parse(int argc, char *argv[]);

extern programmode_t options_mode;
extern char *options_test_param;
extern time_t options_destination_time;
extern time_t options_present_time;
extern time_t options_departed_time;
extern int options_destination_time_valid;
extern int options_present_time_valid;
extern int options_departed_time_valid;
extern int options_utc;
extern int options_relative;
extern int options_hour24;
extern int options_seconds;
extern int options_loop;

#endif