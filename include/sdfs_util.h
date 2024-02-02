#ifndef SDFSUTIL_H
#define SDFSUTIL_H

#include <inttypes.h>

/* write byte chunk from file descriptor */
int64_t sdfs_writechunk(int fd, char *buf, int64_t len);

/* read byte chunk from file descriptor */
int64_t sdfs_readchunk(int fd, char *buf, int64_t len);

#endif
