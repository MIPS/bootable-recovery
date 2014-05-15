/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cutils/log.h"
#include "nand_utils.h"
#include "bootimg.h"

//#define DEBUG_HEADER_DATA

#define NEED_FLASH	1
#define NOT_NEED_FLASH	0

#define MAX_SEGMENT_SIZE  (128 * BUFSIZ) //128k
#define HEADER_SIZE 2048  // size of header to compare for equality

void die(const char *msg, ...) {
	int err = errno;
	va_list args;
	va_start(args, msg);
	char buf[1024];
	vsnprintf(buf, sizeof(buf), msg, args);
	va_end(args);

	if (err != 0) {
		strlcat(buf, ": ", sizeof(buf));
		strlcat(buf, strerror(err), sizeof(buf));
	}

	fprintf(stderr, "%s\n", buf);
	printf("%s\n", buf);
	exit(1);
}

int check(nandContext *part, int image)
{
	int ret = NEED_FLASH;
	int headerlen;
	int checklen;
	char header[HEADER_SIZE];
	char *check;

	lseek(image, 0, SEEK_SET);
	headerlen = read(image, header, HEADER_SIZE);
	if (headerlen <= 0)
		die("nand_flash_image: error reading recovery img header");
	else {
#ifdef DEBUG_HEADER_DATA
		int i;
		printf("DEBUG: nand_flash_image: header, headerlen = %d\n", headerlen);
		for (i = 0; i < HEADER_SIZE;i ++) {
			if (!(i % 16))
				printf("\n%08x:", i);
			printf("  %02x", (unsigned char)header[i]);
		}
		printf("\n");
#endif
	}

	if (posix_memalign((void **)&check, getpagesize(), HEADER_SIZE)) {
		printf("nand_flash_image: alloc memory for check faild!\n");
		goto err;
	}
	lseek(part->fd, 0, SEEK_SET);
	checklen = nand_read_partition(part, check, HEADER_SIZE);
	if (checklen != 0) {
		printf("nand_flash_image: error reading recovery partition, %s\n", strerror(errno));
		goto done;
	} else {
#ifdef DEBUG_HEADER_DATA
		int i;
		printf("DEBUG: nand_flash_image: check!\n");
		for (i = 0; i < HEADER_SIZE; i ++) {
			if (!(i % 16))
				printf("\n%08x:", i);
			printf("  %02x", (unsigned char)check[i]);
		}
		printf("\n");
#endif
	}

	if (!memcmp(header, check, headerlen))
		ret =  NOT_NEED_FLASH;

done:
	free(check);
err:
	return ret;
}

int flash(nandContext *part, int image)
{
	int wrote;
	int len, count = 0;
	char* buffer;

	if (posix_memalign((void **)&buffer, getpagesize(), MAX_SEGMENT_SIZE)) {
		printf("write recovery partition error: alloc memory faild!\n");
		return -1;
	}

	lseek(image, 0, SEEK_SET);
	lseek(part->fd, 0, SEEK_SET);
	while ((len = read(image, buffer, MAX_SEGMENT_SIZE)) > 0) {
		wrote = nand_write_partition(part, buffer, len);
		if (wrote != 0)
			die("write recovery partition error!\n");
		count += len;
	}

	if (len < 0)
		die("read recovery img error!\n");
	else
		printf("update recovery img ok, size = %d\n", count);

	printf("sync filesystem\n");
	sync();

	return 0;
}

/* Read an image file and write it to a flash partition. */
int do_func(char *ptname, char *filename, int ignore_check)
{
	int ret = 0;
	int fd_img = -1;
	nandContext *ctx_part = NULL;

	ctx_part = nand_open_partition(ptname);
	if (ctx_part == NULL)
		die("can't find partition %s", ptname);

	fd_img = open(filename, O_RDONLY);
	if (fd_img < 0)
		die("error opening file %s", filename);

	printf("nand_flash_image: file offset = 0, length = %lu\n", lseek(fd_img, 0, SEEK_END));

	/* If the first part of the file matches the partition, skip writing */
	if ( !ignore_check && (check(ctx_part, fd_img) == NOT_NEED_FLASH))
		printf("image is the same, need not flashing %s\n", ptname);
	else {
		printf("flashing %s from %s\n", ptname, filename);
		ret = flash(ctx_part, fd_img);
	}

	nand_close_partition(ctx_part);
	close(fd_img);

	return ret;
}

int main(int argc, char **argv)
{
//    freopen("/dev/console", "a", stdout); setbuf(stdout, NULL);
 //   freopen("/dev/console", "a", stderr); setbuf(stderr, NULL);
	int ignore_check = 0;
	printf("Start nand_flash_image!\n");

	if (argc < 3) {
		fprintf(stderr, "usage: %s [-i] partition file.img\n", argv[0]);
		return 2;
	}
	if (strncmp(argv[1], "-i", 3) == 0) {
		ignore_check = 1;
	}
	if(do_func(argv[1+ignore_check], argv[2+ignore_check], ignore_check))
		return do_func(argv[1+ignore_check], argv[2+ignore_check], ignore_check);

	return 0;
}

