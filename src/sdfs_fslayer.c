/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#include <sdfs_fslayer.h>
#include <sdfs_util.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <malloc.h>
#include <stdlib.h>

/* error messages definitions */
#define MSG_SUCCESS "fs layer: operation completed"
#define MSG_ERROR "fs layer: operation failure"
#define MSG_EEXIST "fs layer: file or directory already exist"
#define MSG_EACCESS "fs layer: invalid access attempt"
#define MSG_EIO "fs layer: input/output error"
#define MSG_ENOENT "fs layer: file or directory does not exist"
#define MSG_ERENAME "fs layer: file or directory cannot be renamed"
#define MSG_ELISTDIR "fs layer: directory listing failed"
#define MSG_ERMDIR "fs layer: file or directory cannot be removed"

/* file creation function */
sdfs_err sdfs_fsmkfile(sdfs_constr path)
{
    int fd = open(path, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
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
sdfs_err sdfs_fsmkdir(sdfs_constr path)
{
    if (mkdir(path, S_IRWXU) == -1)
        switch (errno) {
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
sdfs_err sdfs_fsrmfile(sdfs_constr path)
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

/* directory deletion function */
sdfs_err sdfs_fsrmdir(sdfs_constr path)
{
    if (rmdir(path) == -1)
        return SDFS_FSERMDIR;
    return SDFS_FSSUCCESS;
}

/* this function may return the block of byte read or n <= -1. n <= -1 means
 * that an error has occured */
sdfs_int64 sdfs_fsreadblk(sdfs_constr path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_RDONLY);
    int64_t rb;
    if (fd != -1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_FSEIO;
        }
        rb = sdfs_readchunk(fd, buf, len);
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
sdfs_int64 sdfs_fswriteblk(sdfs_constr path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len)
{
    int fd = open(path, O_WRONLY);
    int64_t wb;
    if (fd != 1) {
        if (lseek(fd, SEEK_SET, offset) == -1) {
            close(fd);
            return SDFS_FSEIO;
        }
        wb = sdfs_writechunk(fd, buf, len);
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

/* get file or directory statistic */
sdfs_err sdfs_fsgetstat(sdfs_constr path, sdfs_fsstat *stat_obj)
{
    if (stat(path, stat_obj) == -1)
        switch (errno) {
            case ENOENT:
                return SDFS_FSENOENT;
            case EACCES:
                return SDFS_FSEACCESS;
            default:
                return SDFS_FSERROR;
        }
    return SDFS_FSSUCCESS;
}

/* rename file or directory from oldname to newname */
sdfs_err sdfs_fsrename(sdfs_constr old_path, sdfs_constr new_path)
{
    if (rename(old_path, new_path) == -1)
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case EEXIST:
                return SDFS_FSEEXIST;
            case ENOENT:
                return SDFS_FSENOENT;
            case EINVAL: case EISDIR:
                return SDFS_FSERENAME;
            default:
                return SDFS_FSERROR;
        }
    return SDFS_FSSUCCESS;
}

/* list directory function */
sdfs_err sdfs_fslistdir(sdfs_constr path, sdfs_lsdir_clbk callback)
{
    DIR *dd;
    dd = opendir(path);
    struct dirent *dir;
    char fpath[PATH_MAX];
    sdfs_fsclbkctrl ctrl = SDFS_FSCLBKCTRL_CONT;
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (callback) {
                if (dir) {
                    strcpy(fpath, path);
                    strcat(fpath, "/");
                    strcat(fpath, dir->d_name);
                    callback(fpath, &ctrl);
                }
                if (ctrl == SDFS_FSCLBKCTRL_STOP) {
                    closedir(dd);
                    return SDFS_FSSUCCESS;   
                }
            }
            if (!dir) {
                closedir(dd);
                return SDFS_FSSUCCESS;
            }
        } 
    else {
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case EMFILE: case ENFILE: case ENOMEM:
                return SDFS_FSELISTDIR;
            default:
                return SDFS_FSERROR;
        }
    }
}

/* recursively directory list function */
sdfs_err sdfs_fslistdir_r(sdfs_constr path, sdfs_fslsmtsafe *mtsafe,
    sdfs_lsdir_clbk callback)
{
    DIR *dd;    
    if (!mtsafe->init) {
        mtsafe->err = 0;
        if (!realpath(path, mtsafe->canonpath))
            return SDFS_FSELISTDIR;
        if (!strcmp(mtsafe->canonpath, "/")) {
            dd = opendir(mtsafe->canonpath);
            mtsafe->canonpath[0] = '\0';
        } else
            dd = opendir(mtsafe->canonpath);
        mtsafe->init = 1;
    } else 
        dd = opendir(mtsafe->canonpath);
    struct dirent *dir;
    sdfs_fsclbkctrl ctrl = SDFS_FSCLBKCTRL_CONT;
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (dir) {
                strcat(mtsafe->canonpath, "/");
                strcat(mtsafe->canonpath, dir->d_name);
                if (callback)
                    callback(mtsafe->canonpath, &ctrl);
                if (ctrl == SDFS_FSCLBKCTRL_STOP) {
                    closedir(dd);
                    if (mtsafe->err)
                        return SDFS_FSELISTDIR;
                    else
                        return SDFS_FSSUCCESS;
                }
                if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") &&
                    strcmp(dir->d_name, ".."))
                    if (sdfs_fslistdir_r(mtsafe->canonpath, mtsafe, callback) !=
                        SDFS_FSSUCCESS)
                        mtsafe->err++;
                mtsafe->canonpath[strlen(mtsafe->canonpath) - 
                    strlen(dir->d_name) - 1] = '\0';
            } else {
                closedir(dd);
                if (mtsafe->err)
                    return SDFS_FSELISTDIR;
                else
                    return SDFS_FSSUCCESS;
            }
        } 
    else {
        mtsafe->err++;
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case EMFILE: case ENFILE: case ENOMEM:
                return SDFS_FSELISTDIR;
            default:
                return SDFS_FSERROR;
        }
    }
}

/* recursively create directory */
sdfs_err sdfs_fsrmkdir(sdfs_constr path)
{
    char cpath[PATH_MAX];
    strcpy(cpath, path);
    char *pt = strtok(cpath, "/");
    char dir[PATH_MAX];
    strcpy(dir, "/");
    while (pt) {
        strcat(dir, "/");
        strcat(dir, pt);
        mkdir(dir, S_IRWXU);
        pt = strtok(NULL, "/");
    }
    if (access(dir, F_OK))
        return SDFS_FSENOENT;
    return SDFS_FSSUCCESS;
}
    
/* recursively try to remove directories */
sdfs_err sdfs_fsrmdir_r(sdfs_constr path, sdfs_fslsmtsafe *mtsafe)
{
    DIR *dd;    
    if (!mtsafe->init) {
        mtsafe->err = 0;
        if (!realpath(path, mtsafe->canonpath))
            return SDFS_FSERMDIR;
        if (!strcmp(mtsafe->canonpath, "/")) {
            dd = opendir(mtsafe->canonpath);
            mtsafe->canonpath[0] = '\0';
        } else
            dd = opendir(mtsafe->canonpath);
        mtsafe->init = 1;
    } else 
        dd = opendir(mtsafe->canonpath);
    struct dirent *dir;
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (dir) {
                strcat(mtsafe->canonpath, "/");
                strcat(mtsafe->canonpath, dir->d_name);
                if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") &&
                    strcmp(dir->d_name, "..")) {
                    if (sdfs_fsrmdir_r(mtsafe->canonpath, mtsafe) !=
                        SDFS_FSSUCCESS)
                        mtsafe->err++;
                } else if (dir->d_type != DT_DIR)
                    if (unlink(mtsafe->canonpath) == -1)
                        mtsafe->err++;
                mtsafe->canonpath[strlen(mtsafe->canonpath) - 
                    strlen(dir->d_name) - 1] = '\0';
            } else {
                if (rmdir(mtsafe->canonpath) == -1)
                    mtsafe->err++;
                closedir(dd);
                if (mtsafe->err)
                    return SDFS_FSERMDIR;
                else
                    return SDFS_FSSUCCESS;
            }
        } 
    else {
        mtsafe->err++;
        switch (errno) {
            case EACCES:
                return SDFS_FSEACCESS;
            case EMFILE: case ENFILE: case ENOMEM:
                return SDFS_FSERMDIR;
            default:
                return SDFS_FSERROR;
        }
    }
}

/* integer error number to string message */
void sdfs_fsetomsg(const sdfs_err err, const sdfs_pstr str)
{
    switch (err) {
        case SDFS_FSSUCCESS:
            *str = MSG_SUCCESS;
            break;
        case SDFS_FSEEXIST:
            *str = MSG_EEXIST;
            break;
        case SDFS_FSEACCESS:
            *str = MSG_EACCESS;
            break;
        case SDFS_FSERROR:
            *str = MSG_ERROR;
            break;
        case SDFS_FSEIO:
            *str = MSG_EIO;
            break;
        case SDFS_FSENOENT:
            *str = MSG_ENOENT;
            break;
        case SDFS_FSERENAME:
            *str = MSG_ERENAME;
            break;
        case SDFS_FSELISTDIR:
            *str = MSG_ELISTDIR;
            break;
        case SDFS_FSERMDIR:
            *str = MSG_ERMDIR;
            break;
        default:
            *str = MSG_ERROR;
    }
}
