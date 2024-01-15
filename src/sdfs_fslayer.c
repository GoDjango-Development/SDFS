#include <sdfs_fslayer.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* error messages definitions */
#define SDFS_EEXISTMSG "fs layer: file already exist"
#define SDFS_EACCESSMSG "fs layer: invalid access attempt"
#define SDFS_EIOMSG "fs layer: input/output error"

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
                return SDFS_EACCESS;
            case EEXIST:
                return SDFS_EEXIST;
            default:
                return SDFS_ERROR;
        }
    }
    close(fd);
    return SDFS_SUCCESS;
}

/* directory creation function */
sdfs_err sdfs_mkdir(sdfs_str path)
{
    switch (mkdir(path, S_IRWXU)) {
        case EACCES:
            return SDFS_EACCESS;
        case EEXIST:
            return SDFS_EEXIST;
        default:
            return SDFS_ERROR;
    }
    return SDFS_SUCCESS;
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

/* this function may return the block of byte read or n <= -1. n <= -1 meas that
 * an error has occured */
sdfs_int64 sdfs_readblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_RDONLY);
    int64_t rb;
    if (fd != -1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_EIO;
        }
        do
            rb = read(fd, buf, len);
        while (rb == -1 && errno == EINTR);
        close(fd);
    }
    if (rb == -1)
        return SDFS_EIO;
    else
        return rb;
}

/* this function may return the number of bytes written or n <= -1. n <= -1 meas
 * that an error has occured */
sdfs_int64 sdfs_writeblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_WRONLY);
    int64_t wb;
    if (fd != 1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_EIO;
        }
        do
            wb = write(fd, buf, len);
        while (wb == -1 && errno == EINTR);
        close(fd);
    }
    if (wb == -1)
        return SDFS_EIO;
    else
        return wb;
}

/* integer error number to string message */
void sdfs_etomsg(sdfs_err err, sdfs_str str)
{
    switch (err) {
        case SDFS_SUCCESS:
            strcpy(str, SDFS_SUCCESSMSG);
            break;
        case SDFS_EEXIST:
            strcpy(str, SDFS_EEXISTMSG);
            break;
        case SDFS_EACCESS:
            strcpy(str, SDFS_EACCESSMSG);
            break;
        case SDFS_ERROR:
            strcpy(str, SDFS_ERRORMSG);
            break;
        case SDFS_EIO:
            strcpy(str, SDFS_EIOMSG);
    }
}

/* file and directory deletion function */
static sdfs_err sdfs_rment(sdfs_str path)
{
    if (unlink(path) == -1)
        switch (errno) {
            case EACCES:
                return SDFS_EACCESS;
            case ENOENT:
                return SDFS_ENOENT;
            default:
                return SDFS_ERROR;
        }
    return SDFS_SUCCESS;
}
