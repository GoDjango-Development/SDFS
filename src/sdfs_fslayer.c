#include <sdfs_fslayer.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define SDFS_EEXISTMSG "fs layer: file already exist"
#define SDFS_EACCESSMSG "fs layer: invalid access attempt"

sdfs_err sdfs_mkfile(sdfs_str path)
{
    int fd = open(path, O_CREAT | O_EXCL, S_IRWXU);
    if (fd == -1)
        switch (errno) {
            case EACCES:
                return SDFS_EACCESS;
                break;
            case EEXIST:
                return SDFS_EEXIST;
                break;
            default:
                return SDFS_ERROR;
                break;
        }
    return SDFS_SUCCESS;
}

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
