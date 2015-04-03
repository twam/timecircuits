#include "spi.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static const char *device = "/dev/spidev0.0";
//static uint8_t spi_mode = 0;
static uint8_t spi_bits = 8;
static uint32_t spi_speed = 500e3;
static uint16_t spi_delay = 0;
static int spi_fd;

static void pabort(const char *s)
{
	perror(s);
	abort();
}

void spi_init()
{

	spi_fd = open(device, O_RDWR);
	if (spi_fd < 0)
		pabort("can't open device");
}

void spi_exit()
{
		close(spi_fd);

}

void spi_transfer(uint16_t *buf, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (uint32_t)buf,
		.rx_buf = (uint32_t)NULL,
		.len = len*sizeof(uint16_t),
		.delay_usecs = spi_delay,
		.speed_hz = spi_speed,
		.bits_per_word = spi_bits,
	};

	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");
}