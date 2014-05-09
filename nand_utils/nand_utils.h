#ifndef NAND_UTILS_H_
#define NAND_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h>  // for size_t, etc.

#define ND_ERASE_HARD	0x1
#define ND_ERASE_SOFT	0x2

typedef struct NandContext {
	ssize_t capacity;
    int fd;
} nandContext;

int nand_erase_partition(const char *name, int flag);
int nand_erase_all_partitions(int flag);
int nand_prepare_new_flash(void);
int nand_check_used_flash(void);
int nand_install_partitions(const char *cmd);

nandContext *nand_open_partition(const char *name);
void nand_close_partition(nandContext *ctx);
int nand_write_partition(const nandContext *ctx,char *srcbuf,ssize_t len);
int nand_read_partition(const nandContext *ctx,char *dstbuf,ssize_t len);

#ifdef __cplusplus
}
#endif


#endif  //NAND_UTILS_H_

