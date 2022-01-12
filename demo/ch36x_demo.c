/* 
 * ch365/ch367/ch368 PCI/PCIE application example
 *
 * Copyright (C) 2021 WCH.
 * Author: TECH39 <zhangj@wch.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I /path/to/cross-kernel/include
 *
 * Version: V1.11
 * 
 * Update Log:
 * V1.0 - initial version
 * V1.1 - called new APIs within ch36x_lib
 * V1.11 - called SPI APIs within ch36x_lib
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>   
#include <string.h>
#include <sys/types.h> 
#include "../lib/ch36x_lib.h"

static const char *device = "/dev/ch36xpci0";
static int fd;
static unsigned long iobase;
static unsigned long membase;
static int mOffset=0; // current A0~A15
static const int ValidAddr=0x7FFF; // current A0~A14


static void ch36x_demo_io_operate(void)
{
	int ret;
	char c;
	int ibyte;
	int offset;
	uint8_t obyte;

	printf("\n---------- IO read write test ----------\n");	
	while (1) {
		printf("press w to write one byte, r to read one byte, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		switch (c) {
		case 'w':
			printf("input offset of io:\n");
			scanf("%x", &offset);
			getchar();
			printf("input write value:\n");
			scanf("%x", &ibyte);
			getchar();
			ret = ch36x_write_io_byte(fd, iobase + offset, (uint8_t)ibyte);
			if (ret != 0)
				printf("io write fail.\n");
			break;
		case 'r':
			printf("input offset of io:\n");
			scanf("%x", &offset);
			getchar();
			ret = ch36x_read_io_byte(fd, iobase + offset, &obyte);
			if (ret != 0)
				printf("io read fail.\n");
			printf("read byte: 0x%2x\n", obyte);
			break;
		default:
			break;
		}
	}
}

static void ch36x_demo_mem_operate(void)
{
	int ret;
	char c;
	int ibyte;
	int offset;
	uint8_t obyte;

	printf("\n---------- Memory read write test ----------\n");	
	while (1) {
		printf("press w to write one byte, r to read one byte, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		switch (c) {
		case 'w':
			printf("input offset of mem:\n");
			scanf("%x", &offset);
			getchar();
			printf("input write value:\n");
			scanf("%x", &ibyte);
			getchar();
			ret = ch36x_write_mem_byte(fd, membase + offset, (uint8_t)ibyte);
			if (ret != 0)
				printf("memory write fail.\n");
			break;
		case 'r':
			printf("input offset of mem:\n");
			scanf("%x", &offset);
			getchar();
			ret = ch36x_read_mem_byte(fd, membase + offset, &obyte);
			if (ret != 0)
				printf("memory read fail.\n");
			printf("read byte: 0x%2x\n", obyte);
			break;
		default:
			break;
		}
	}
}

static void ch36x_demo_dio_operate(void)
{
	int ret;
	char c;
	int ibyte;
	int obyte;
	uint8_t temp;

	printf("\n---------- DO:16 DI:32 test ----------\n");	
	while (1) {
		printf("press w to output, r to input, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		switch (c) {
		case 'w':
			printf("output value(0x0000~0xffff):\n");
			scanf("%x", &mOffset);
			getchar();
			//Set A15
			ch36x_read_io_byte(fd, iobase + 0xF8, &temp);
			ch36x_write_io_byte(fd, iobase + 0xF8, (temp & 0xfe) | ((mOffset & 0x8000) > 15));
			//Set A0~A14
			ret = ch36x_read_mem_dword(fd, membase+(mOffset&ValidAddr), &ibyte);
			if (ret != 0)
				printf("memory write fail.\n");
			break;
		case 'r':
			ret = ch36x_read_mem_dword(fd,( membase+(mOffset&ValidAddr)), &obyte);
			if (ret != 0)
				printf("memory read fail.\n");
			printf("read byte: 0x%2x\n", obyte);
			break;
		default:
			break;
		}
	}
}

static void ch36x_demo_48do_operate(void)
{
	int ret;
	char c;
	uint64_t obyte;
	uint8_t ibyte;

	printf("\n---------- DO:48 test ----------\n");
	while (1)
	{
		printf("press w to output, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		printf("output value:\n");
		scanf("%lx", &obyte);
		getchar();
		if (obyte > 0xffffffffffff)
		{
			printf("out of resource\n");
			continue;
		}
		//set a15 first
		ch36x_read_io_byte(fd,iobase+0xF8,&ibyte);
		ch36x_write_io_byte(fd,iobase+0xF8,(ibyte&0xfe)|((obyte&0x8000)>>15));

		//set A0~A14 and D0~D31
		ret = ch36x_write_mem_dword(fd, membase + (uint32_t)(obyte&ValidAddr), (uint32_t)((obyte&0xffffffff0000)>>16));
		if (ret != 0)
			printf("memory write fail.\n");
	}
}
 
static void ch36x_demo_config_operate(void)
{
	int ret;
	char c;
	int ibyte;
	int offset;
	uint8_t obyte;

	printf("\n---------- Config space read write test ----------\n");	
	while (1) {
		printf("press w to write one byte, r to read one byte, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		switch (c) {
		case 'w':
			printf("input offset of config space:\n");
			scanf("%x", &offset);
			getchar();
			printf("input write value:\n");
			scanf("%x", &ibyte);
			getchar();
			ret = ch36x_write_config_byte(fd, offset, ibyte);
			if (ret != 0)
				printf("config space write fail.\n");
			break;
		case 'r':
			printf("input offset of config space:\n");
			scanf("%x", &offset);
			getchar();
			ret = ch36x_read_config_byte(fd, offset, &obyte);
			if (ret != 0)
				printf("config space read fail.\n");
			printf("read byte: 0x%2x\n", obyte);
			break;
		default:
			break;
		}
	}
}

static void ch36x_dmeo_isr_handler(void)
{
	static int int_times = 0;
	int ret;
	uint32_t obyte;


	printf("ch36x interrupt times: %d\n", int_times++);
	ret = ch36x_read_mem_dword(fd, membase + mOffset, &obyte);
	if (ret != 0)
		printf("memory read fail.\n");
	printf("read byte: 0x%8x\n", obyte);
}

static void ch36x_demo_isr_enable(void)
{
	int ret;
	enum INTMODE mode = INT_FALLING;

	ret = ch36x_enable_isr(fd, mode);
	if (ret != 0) {
		printf("ch36x_enable_isr failed.\n");
		return;
	}
	ch36x_set_int_routine(fd, ch36x_dmeo_isr_handler);
}

static void ch36x_demo_isr_disable(void)
{
	int ret;

	ret = ch36x_disable_isr(fd);
	if (ret != 0) {
		printf("ch36x_disable_isr failed.\n");
		return;
	}
	ch36x_set_int_routine(fd, NULL);
}

static void ch36x_demo_spi_operate(void)
{
	/* bit0 of mode on SPI Freq, 0->31.3MHz, 1->15.6MHz */
	/* bit1 of mode on SPI I/O Pinout, 0->SPI3(SCS/SCL/SDX), 1->SPI4(SCS/SCL/SDX/SDI) */
	int mode = 0x03;
	int ret;
	int ilen, olen;
	uint8_t ibuffer[1024];
	uint8_t obuffer[1024];
	int i;

	printf("\n---------- SPI read write test ----------\n");
	ret = ch36x_set_stream(fd, mode);
	if (ret) {
		printf("set stream error.\n");
		return;
	}
	printf("input write length:\n");
	scanf("%d", &ilen);
	getchar();
	printf("input read length:\n");
	scanf("%d", &olen);
	getchar();
	memset(ibuffer, 0x55, sizeof(ibuffer));
	ret = ch36x_stream_spi(fd, ibuffer, ilen, obuffer, olen);
	if (ret != 0) {
		printf("spi transfer fail.\n");
		return;
	}
	printf("\n---------- read buffer ----------\n");
	for (i = 0; i < olen; i++) {
		printf("\tobuffer[%d]: 0x%2x\n", i, obuffer[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	// int fd;
	int ret;
	char c;
	enum CHIP_TYPE chiptype;
	// unsigned long iobase;
	// unsigned long membase;

	fd = ch36x_open(device);
	if (fd < 0) {
		printf("ch36x_open error.\n");
		goto exit;
	}

	ret = ch36x_get_chiptype(fd, &chiptype);
	if (ret != 0) {
		printf("ch36x_get_chiptype error.\n");
		goto exit;
	}
	switch (chiptype) {
	case CHIP_CH365:
		printf("current chip model: CH365.\n");
		break;
	case CHIP_CH367:
		printf("current chip model: CH367.\n");
		break;
	case CHIP_CH368:
		printf("current chip model: CH368.\n");
		break;
	}

	ret = ch36x_get_ioaddr(fd, &iobase);
	if (ret != 0) {
		printf("ch36x_get_ioaddr error.\n");
		goto exit;
	}
	printf("iobase:%lx\n", iobase);

	if (chiptype == CHIP_CH368) {
		ret = ch36x_get_memaddr(fd, &membase);
		if (ret != 0) {
			printf("ch36x_get_memaddr error.\n");
			goto exit;
		}
		printf("membase:%lx\n", membase);
	}
	
	while (1) {
		printf("press c to operate config space, m to operate memory space, "
				"i to operate io space, e to enable interrupt, "
				"d to disable interrpt, s to operate spi, q for quit.\n");
		scanf("%c", &c);
		getchar();
		if (c == 'q')
			break;
		switch (c) {
		case 'i':
			ch36x_demo_io_operate();
			break;
		case 'm':
			if (chiptype == CHIP_CH368)
				ch36x_demo_mem_operate();
			else
				printf("chip not support.\n");
			break;
		case 'c':
			ch36x_demo_config_operate();
			break;
		case 'e':
			ch36x_demo_isr_enable();
			break;
		case 'd':
			ch36x_demo_isr_disable();
			break;
		case 's':
			ch36x_demo_spi_operate();
			break;
		case 'I':
			ch36x_demo_dio_operate();
			break;
		case 'O':
			ch36x_demo_48do_operate();
			break;
		default:
			break;
		}
	}
 	
	ret = ch36x_close(fd);
	if (ret != 0) {
		printf("ch36x_close error.\n");
		goto exit;
	}

exit:
	return ret;
}
