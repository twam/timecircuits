#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define COLOR_RESET "\e[0m"
#define COLOR_BOLDWHITE "\e[1m"
#define COLOR_BOLDRED "\e[1;31m"
#define COLOR_BOLDGREEN "\e[2;31m"
#define COLOR_BOLDYELLOW "\e[3;31m"
#define COLOR_GRAY "\e[0;37m"

// red		destination time
// green	present time
// yellow	last time departed

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define	CHIPS_PER_MODULE		14
#define CHARACTER_SEGMENTS_PER_MODULE	13
#define MODULES				3
#define CHIPS_TOTAL			(MODULES*CHIPS_PER_MODULE)
#define CHARACTER_SEGMENTS_TOTAL 	(MODULES*CHARACTER_SEGMENTS_PER_MODULE)

#define LED_CHIP			9
#define LED_PM_BIT			0
#define LED_AM_BIT			1
#define LED_SECONDS_UPPER_BIT		2
#define LED_SECONDS_LOWER_BIT		3
#define LED_PM				(1<<LED_PM_BIT)
#define LED_AM				(1<<LED_AM_BIT)
#define LED_SECONDS			((1<<LED_SECONDS_LOWER_BIT) | (1<<LED_SECONDS_UPPER_BIT))

uint16_t charset[128] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //                 
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //                 
	0x0000, 0x0000, 0x8008, 0x0000, 0xB9D3, 0xFCF5, 0xCB85, 0x8000, 0x0024, 0x4400, 0xDC34, 0x9810, 0x0020, 0x1010, 0x0100, 0x0404, //  !"#$%&'()*+,-./
	0x23CB, 0x0048, 0x139B, 0x11DB, 0x3058, 0x31D3, 0x33D3, 0x004B, 0x33DB, 0x31DB, 0x1100, 0x1400, 0x0024, 0x1190, 0x4400, 0x081B, // 0123456789:;<=>?
	0x239F, 0x325B, 0x89DB, 0x2383, 0x89CB, 0x3383, 0x3203, 0x23D3, 0x3258, 0x8983, 0x03C8, 0x3224, 0x2380, 0x624C, 0x6268, 0x23CB, // @ABCDEFGHIJKLMNO
	0x321B, 0x23EB, 0x323B, 0x31D3, 0x8803, 0x23C8, 0x2604, 0x2668, 0x4424, 0x4804, 0x0587, 0x8882, 0x4020, 0x8901, 0x0000, 0x0180, // PQRSTUVWXYZ[\]^_
	0x4000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // `abcdefghijklmno
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9882, 0x8800, 0x8911, 0x0000, 0x0000, // pqrstuvwxyz{|}~
};

void clear_display_data(uint16_t* display_data)
{
	memset(display_data, 0, sizeof(*display_data)*CHIPS_TOTAL);
}

void string_to_display_data(uint16_t* display_data, size_t offset, const char* buf)
{
	// current position at the 'screen', counting from top left
	size_t pos = offset;
	// current position within buffer
	size_t buf_pos = 0;

	while ((buf[buf_pos] != 0) && (pos < CHARACTER_SEGMENTS_TOTAL)) {
		// current line (counting from top)
		size_t line = pos / CHARACTER_SEGMENTS_PER_MODULE;
		// current position within line (counting from left)
		size_t pos_line = pos % CHARACTER_SEGMENTS_PER_MODULE;

		// calculate corresponding chip
		size_t chip_pos = (CHIPS_TOTAL - 1) - ((MODULES - 1) - line) * CHIPS_PER_MODULE - (pos_line < LED_CHIP ? pos_line : pos_line + 1) ;

		char character = toupper(buf[buf_pos]);

		if (character > ARRAY_SIZE(charset)) {
			character = '?';
		}

		display_data[chip_pos] = charset[(size_t)character];

		buf_pos++;
		pos++;
	}
}

void time_to_display_data(uint16_t* display_data, int display, struct tm* tm, int hour24, int seconds)
{
	char buffer[CHARACTER_SEGMENTS_TOTAL+1];

	strftime(buffer, sizeof(buffer), hour24 ? "%b%d%Y%H%M" : "%b%d%Y%I%M", tm);

	string_to_display_data(display_data, CHARACTER_SEGMENTS_PER_MODULE*display, buffer);

	size_t seconds_chip_pos = (CHIPS_TOTAL - 1) - ((MODULES - 1) - display) * CHIPS_PER_MODULE - LED_CHIP;

	if (!hour24) {
		display_data[seconds_chip_pos] |= (tm->tm_hour < 12) ? LED_AM : LED_PM;
	}

	if ((seconds) && (tm->tm_sec % 2)) {
		display_data[seconds_chip_pos] |= LED_SECONDS;
	}
}

void print_display_data(uint16_t* display_data)
{
	size_t i;

	for (i = 0; i < CHIPS_TOTAL; ++i) {
		printf("%04X ", display_data[i]);
		if ((i+1) % CHIPS_PER_MODULE == 0)
			printf("\n");
	}
	printf("\n");
}

#if 1
int main(int argc, char *argv[])
{
	uint16_t display_data[CHIPS_TOTAL];
	int ret = 0;
	int fd;
	int i;

	char buf[CHIPS_TOTAL+1];
	snprintf(buf, CHIPS_TOTAL+1, "OBEN     OBENMITTE   MITTEUNTEN   UNTEN");


#if 0
	for (i = 0; i < CHIPS_TOTAL; ++i) {
		display_data[i] = 0x0001;
	}
#endif

#if 0
	clear_display_data(display_data);
	string_to_display_data(display_data, 0*CHARACTER_SEGMENTS_PER_MODULE, buf);
	print_display_data(display_data);
#endif

#if 1
	time_t timer;
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	clear_display_data(display_data);
	time_to_display_data(display_data, 0, tm_info, 1, 1);

	tm_info->tm_year -= 30;
	time_to_display_data(display_data, 1, tm_info, 1, 1);


	tm_info->tm_year += 60;
	time_to_display_data(display_data, 2, tm_info, 1, 1);
	print_display_data(display_data);
#endif
	spi_init();

	spi_transfer(display_data, CHIPS_TOTAL);

	spi_exit();

	return ret;
}
#endif
