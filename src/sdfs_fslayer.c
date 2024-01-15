#include <sdfs_fslayer.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* error messages definitions */
#define MSG_SUCCESS "fs layer: operation completed"
#define MSG_ERROR "fs layer: operation failure"
#define MSG_EEXIST "fs layer: file already exist"
#define MSG_EACCESS "fs layer: invalid access attempt"
#define MSG_EIO "fs layer: input/output error"
#define MSG_ENOENT "fs layer: file or directory does not exist"

/* file and directory deletion function */
static sdfs_err sdfs_rment(sdfs_str path);

/* file creation function */
sdfs_err sdfs_mkfile(sdfs_str path)
{
    int fd = open(path, O_CREAT | O_EXCL, S_IRWXU);
    if (fd == -1) {
        close(fd);
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case EEXIST:
                return SDFS_FSEEXIST;
            default:
                return SDFS_FSERROR;
        }
    }
    close(fd);
    return SDFS_FSSUCCESS;
}

/* directory creation function */
sdfs_err sdfs_mkdir(sdfs_str path)
{
    switch (mkdir(path, S_IRWXU)) {
        case EACCES:
            return SDFS_FSEACCESS;
        case EEXIST:
            return SDFS_FSEEXIST;
        default:
            return SDFS_FSERROR;
    }
    return SDFS_FSSUCCESS;
}

/* file deletion function */
sdfs_err sdfs_rmfile(sdfs_str path)
{
    return sdfs_rment(path);
}

/* directory deletion function */
sdfs_err sdfs_rmdir(sdfs_str path)
{
    return sdfs_rment(path);
}

/* this function may return the block of byte read or n <= -1. n <= -1 means
 * that an error has occured */
sdfs_int64 sdfs_readblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_RDONLY);
    int64_t rb;
    if (fd != -1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_FSEIO;
        }
        do
            rb = read(fd, buf, len);
        while (rb == -1 && errno == EINTR);
        close(fd);
    }
    if (rb == -1 || fd == -1)
        switch (errno) {
            /* open error checking */
            case EACCES:
                return SDFS_FSEACCESS;
            case ENOENT:
                return SDFS_FSENOENT;
            /* read error checking */
            case EIO: case EINVAL:
                return SDFS_FSEIO;
            default:
                return SDFS_FSERROR;
        }
    return rb;
}

/* this function may return the number of bytes written or n <= -1. n <= -1
 * means that an error has occured */
sdfs_int64 sdfs_writeblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_WRONLY);
    int64_t wb;
    if (fd != 1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_FSEIO;
        }
        do
            wb = write(fd, buf, len);
        while (wb == -1 && errno == EINTR);
        close(fd);
    }
    if (wb == -1 || fd == -1)
        switch (errno) {
            /* open error checking */
            case EACCES:
                return SDFS_FSEACCESS;
            case ENOENT:
                return SDFS_FSENOENT;
            /* write error checking */
            case EIO: case EFBIG: case ENOSPC: case EINVAL:
                return SDFS_FSEIO;
            default:
                return SDFS_FSERROR;
        }
    return wb;
}

/* integer error number to string message */
void sdfs_etomsg(sdfs_err err, sdfs_str str)
{
    switch (err) {
        case SDFS_FSSUCCESS:
            strcpy(str, MSG_SUCCESS);
            break;
        case SDFS_FSEEXIST:
            strcpy(str, MSG_EEXIST);
            break;
        case SDFS_FSEACCESS:
            strcpy(str, MSG_EACCESS);
            break;
        case SDFS_FSERROR:
            strcpy(str, MSG_ERROR);
            break;
        case SDFS_FSEIO:
            strcpy(str, MSG_EIO);
            break;
        case SDFS_FSENOENT:
            strcpy(str, MSG_ENOENT);
            break;
        default:
            strcpy(str, MSG_ERROR);
    }
}

/* file or directory deletion function */
static sdfs_err sdfs_rment(sdfs_str path)
{
    if (unlink(path) == -1)
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case ENOENT:
                return SDFS_FSENOENT;
            default:
                return SDFS_FSERROR;
        }
    return SDFS_FSSUCCESS;
}
