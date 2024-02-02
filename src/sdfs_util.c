#include <sdfs_util.h>
#include <unistd.h>
#include <errno.h>

/* write byte chunk from file descriptor */
int64_t sdfs_writechunk(int fd, char *buf, int64_t len)
{
    int64_t wb;
    int64_t written = 0;
    do {
        do 
            wb = write(fd, buf + written, len - written);
        while (wb == -1 && errno == EINTR);
        written += wb;
    } while (wb != -1 && len - written > 0);
    return wb;
}

/* read byte chunk from file descriptor */
int64_t sdfs_readchunk(int fd, char *buf, int64_t len)
{
    int64_t rb;
    do 
        rb = read(fd, buf, len);
    while (rb == -1 && errno == EINTR);
    return rb;
}
