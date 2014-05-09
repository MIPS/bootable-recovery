/*
 * Copyright (C) 2012 Ingenic
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cutils/log.h"
#include "nand_utils.h"

#define FILE_X_BOOT "/mnt/sdcard/x-boot.bin"
#define FILE_BOOT "/mnt/sdcard/boot.img"
#define FILE_SYSTEM "/mnt/sdcard/system.img"
#define FILE_DATA "/mnt/sdcard/userdata.img"
#define FILE_MISC "/data/misc.img"

enum debug_type {
	READ_MODE,
	WRITE_MODE,
	RW_MODE,
};

#define DUMP_MODE 		READ_MODE
#define SRC_FILE		FILE_BOOT
#define DST_FILE		"/sdcard/dump.img"
#define DUMP_PARTITION	"boot"
#define READ_CNT 2560 /* 16 * 1024 */

static int nand_read(nandContext* ctx, char* filename, char* buffer, int read_cnt) {
	int read, written;
	FILE* f;
	int i = 0;
	int pos = 0;
	int print_cnt = 4;

	/* partition->buffer */
	memset(buffer, 0, BUFSIZ);
	lseek(ctx->fd, 0, SEEK_SET);

	f = fopen(filename, "w");
	if (f)
		fclose(f);
	else {
		printf("nand_test can't open/create file %s\n", filename);
		return -1;
	}

	f = fopen(filename, "r+");
	if (f) {
		/*
		pos = lseek(f, 0, SEEK_SET);
		if (pos == -1) {
			printf("nand_test lseek %s failed!\n", filename);
			fclose(f);
			return -1;
		}
		*/

		while (read_cnt --) {
			read = nand_read_partition(ctx, buffer, BUFSIZ);
			if (read != 0) {
				printf("nand_test read data failed!\n");
				fclose(f);
				return -1;
			}

			if (print_cnt-- > 0) {
				/* print buffer */
				printf("==========================================\n");
				for (i = 0; i < BUFSIZ; i++) {
					if (!(i % 16))
						printf("\n[%08x]: ", i);
					printf("%02x ", (unsigned char)buffer[i]);
				}
				printf("\n=========================================\n");
			}

			written = fwrite(buffer, 1, BUFSIZ, f);
			if (written < 0) {
				printf("wrrite file %s error, ret = %d!\n", filename, written);
				fclose(f);
				return -1;
			} else
				printf("%s, written bytes = %d\n", filename, written);
		}
		fclose(f);
	} else
		printf("open file %s failed!\n", filename);

	return 0;
}

static int nand_write(nandContext* ctx, char* filename, char* buffer) {
	FILE* f;
	int read, written;
	unsigned int count = 0;

	/* file->partition */
	f = fopen(filename, "rb");
	if (f) {
		printf("open file %s ok!\n", filename);
		lseek(ctx->fd, 0, SEEK_SET);
		while ((read = fread(buffer, 1, BUFSIZ, f)) > 0) {
			written = nand_write_partition(ctx, buffer, read);
			if (written != 0) {
				printf("nand_test write data failed!\n");
				fclose(f);
				return -1;
			} else
				printf("written = %d\n", read);

			count += read;
		}
		fclose(f);
	} else
		printf("open file %s failed!\n", filename);

	printf("Write partition ok, written = %d!\n", count);

	return 0;
}

int main(int argc, char **argv) {
	nandContext *ctx = NULL;
	char* buffer = malloc(BUFSIZ);
    int ret;

	/* open partition */
	ctx = nand_open_partition(DUMP_PARTITION);
	if (ctx == NULL) {
		printf("open nand partition %s failed", DUMP_PARTITION);
		return -1;
	} else
		printf("open nand partition %s ok!\n", DUMP_PARTITION);

	switch (DUMP_MODE)
	{
	case READ_MODE:
		ret = nand_read(ctx, DST_FILE, buffer, READ_CNT);
		break;
	case WRITE_MODE:
		ret = nand_write(ctx, SRC_FILE, buffer);
		break;
	case RW_MODE:
		ret = nand_write(ctx, SRC_FILE, buffer);
		if (!ret)
			ret = nand_read(ctx, DST_FILE, buffer, READ_CNT);
		break;
	default:
		printf("nand_test: unknonw debug mode!\n");
	}

	printf("close nand partition ...\n");
	nand_close_partition(ctx);
	free(buffer);

	printf("exit nand_test\n");

	return 0;
}
