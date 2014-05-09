/*
 * Copyright (C) 2012 Ingenic
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>  // for _IOW, _IOR, mount()
#include <sys/stat.h>
#include <mtd/mtd-user.h>
#include <assert.h>

#include "nand_utils.h"

#define	CMD_PREPARE_NEW_NAND		94
#define	CMD_CHECK_USED_NAND		95
#define	CMD_SOFT_ERASE_PARTITION	96
#define	CMD_SOFT_ERASE_ALL_PARTITIONS	97
#define CMD_ERASE_PARTITION		98
#define CMD_ERASE_ALL_PARTITIONS	99

#define CHAR_NAND_DRIVER 	   	"/dev/nand_char"

static int nand_ioctl(int cmd, const char *arg) {
	int ret = 0;
	int fd = open(CHAR_NAND_DRIVER, O_RDWR);
	if (fd < 0) {
		printf("nand_ioctl error: can't open nand device %s!\n", CHAR_NAND_DRIVER);
		return -1;
	}

	if ((ret = ioctl(fd, cmd, arg))) {
		printf("nand_ioctl error:  arg = %s, ret = %d\n", arg, ret);
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

int nand_erase_partition(const char *name, int flag) {
	if (flag == ND_ERASE_HARD)
		return nand_ioctl(CMD_ERASE_PARTITION, name);
	else
		return nand_ioctl(CMD_SOFT_ERASE_PARTITION, name);
}

int nand_erase_all_partitions(int flag) {
	if (flag == ND_ERASE_HARD)
		return nand_ioctl(CMD_ERASE_ALL_PARTITIONS, NULL);
	else
		return nand_ioctl(CMD_SOFT_ERASE_ALL_PARTITIONS, NULL);
}

int nand_prepare_new_flash()
{
	return nand_ioctl(CMD_PREPARE_NEW_NAND, NULL);
}

int nand_check_used_flash()
{
	return nand_ioctl(CMD_CHECK_USED_NAND, NULL);
}

int nand_install_partitions(const char *cmd) {
	int fd = open(CHAR_NAND_DRIVER, O_RDWR);
	if (fd < 0) {
		printf("nand_install_partitions error: can't open nand device %s!\n", CHAR_NAND_DRIVER);
		return -1;
	} else
		printf("nand_install_partitions: open device %s ok!\n", CHAR_NAND_DRIVER);

	if (write(fd, cmd, strlen(cmd)) <= 0) {
		printf("nand_install_partitions error: install nand partition faild!\n");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}

nandContext *nand_open_partition(const char *name)
{
	char dev_name[128];
	nandContext *ctx = (nandContext *)malloc(sizeof(nandContext));

	if (strlen(name) >= (128 - strlen("/dev/block/"))) {
		printf("nand_open_partition error: partition name is too long\n");
		return NULL;
	}

	sprintf(dev_name, "/dev/block/%s", name);
	ctx->fd = open(dev_name, O_RDWR | O_DIRECT);
	if (ctx->fd < 0) {
		printf("nand_open_partition error: can't open nand device %s!\n", dev_name);
		free(ctx);
		return NULL;
	} else
		printf("nand_open_partition: open device '%s' ok!\n", dev_name);

	return ctx;
}

void nand_close_partition(nandContext *ctx)
{
	fsync(ctx->fd);
	close(ctx->fd);
	free(ctx);
}

int nand_write_partition(const nandContext *ctx,char *srcbuf,ssize_t len)
{
	int count = 0, written = 0;
	off_t pos = lseek(ctx->fd, 0, SEEK_CUR);

	if (pos == (off_t) -1) {
		printf("nand_write_partition error: pos == -1!\n");
		return -1;
	}

	if (srcbuf == NULL/* || pos+len > ctx->capacity*/) {
		printf("nand_write_partition error: srcbuf == NULL!\n");
		return -1;
	}

	while(count < len) {
		written = write(ctx->fd, srcbuf + count, len-count);
		if (written < 0) {
			printf("nand_write_partition error: ret == %d, %s!\n", written, strerror(errno));
			return -1;
		}
		count += written;
	}

	return 0;
}

int nand_read_partition(const nandContext *ctx,char *dstbuf,ssize_t len)
{
	int count = 0;
	off_t pos = lseek(ctx->fd, 0, SEEK_CUR);
	if (pos == (off_t) -1) {
		printf("nand_read_partition error: pos == -1!\n");
		return -1;
	}

	if (dstbuf == NULL) {
		printf("nand_read_partition error: dstbuf == NULL!\n");
		return -1;
	}

	count = read(ctx->fd,dstbuf,len);
	if (count <= 0) {
		printf("nand_read_partition error: ret == %d, %s!\n", count, strerror(errno));
		return -1;
	}

	return 0;
}
