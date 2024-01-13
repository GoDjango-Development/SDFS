#include <sdfs_fslayer.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* error messages definitions */
#define SDFS_EEXISTMSG "fs layer: file already exist"
#define SDFS_EACCESSMSG "fs layer: invalid access attempt"

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

/* file and directory deletion function */
sdfs_err sdfs_rment(sdfs_str path)
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
    }
}
