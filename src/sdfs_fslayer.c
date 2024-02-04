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

/* file and directory deletion function */
static sdfs_err sdfs_rment(const sdfs_str path);

/* file creation function */
sdfs_err sdfs_mkfile(const sdfs_str path)
{
    int fd = open(path, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
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
sdfs_err sdfs_mkdir(const sdfs_str path)
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
sdfs_err sdfs_rmfile(const sdfs_str path)
{
    return sdfs_rment(path);
}

/* directory deletion function */
sdfs_err sdfs_rmdir(const sdfs_str path)
{
    return sdfs_rment(path);
}

/* this function may return the block of byte read or n <= -1. n <= -1 means
 * that an error has occured */
sdfs_int64 sdfs_readblk(const sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
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
sdfs_int64 sdfs_writeblk(const sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
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
sdfs_err sdfs_getstat(const sdfs_str path, sdfs_stat *stat_obj)
{
    int rc = stat(path, stat_obj);
    if (rc == -1)
        switch (errno) {
            case ENOENT:
                return SDFS_FSENOENT;
            default:
                return SDFS_FSERROR;
        }
    return SDFS_FSSUCCESS;
}

/* rename file or directory from oldname to newname */
sdfs_err sdfs_rename(const sdfs_str old_path, const sdfs_str new_path)
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
sdfs_err sdfs_listdir(const sdfs_str path, sdfs_lsdir_clbk callback)
{
    DIR *dd;
    dd = opendir(path);
    struct dirent *dir;
    sdfs_clbkctrl ctrl = SDFS_CLBKCTRL_CONT;
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (callback) {
                if (dir)
                    callback(dir->d_name, &ctrl);
                else
                    callback(NULL, &ctrl);
                if (ctrl == SDFS_CLBKCTRL_STOP) {
                    closedir(dd);
                    return SDFS_FSSUCCESS;   
                }
            }
            if (!dir) {
                closedir(dd);
                if (callback)
                    callback(NULL, &ctrl);
                return SDFS_FSSUCCESS;
            }
        } 
    else {
        if (callback)
            callback(NULL, &ctrl);
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
sdfs_err sdfs_listdir_r(const sdfs_str path, lsdir_mtsafe *mtsafe,
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
    sdfs_clbkctrl ctrl = SDFS_CLBKCTRL_CONT;
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (dir) {
                strcat(mtsafe->canonpath, "/");
                strcat(mtsafe->canonpath, dir->d_name);
                if (callback)
                    callback(mtsafe->canonpath, &ctrl);
                if (ctrl == SDFS_CLBKCTRL_STOP) {
                    closedir(dd);
                    if (mtsafe->err)
                        return SDFS_FSELISTDIR;
                    else
                        return SDFS_FSSUCCESS;
                }
                if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") &&
                    strcmp(dir->d_name, ".."))
                    if (sdfs_listdir_r(mtsafe->canonpath, mtsafe, callback) !=
                        SDFS_FSSUCCESS) {
                        if (callback)
                            callback(NULL, &ctrl);
                        mtsafe->err++;
                    }
                mtsafe->canonpath[strlen(mtsafe->canonpath) - 
                    strlen(dir->d_name) - 1] = '\0';
            } else {
                closedir(dd);
                if (callback)
                    callback(NULL, &ctrl);
                if (mtsafe->err)
                    return SDFS_FSELISTDIR;
                else
                    return SDFS_FSSUCCESS;
            }
        } 
    else {
        if (callback)
            callback(NULL, &ctrl);
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

/* integer error number to string message */
void sdfs_fsetomsg(const sdfs_err err, sdfs_str str)
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
        case SDFS_FSERENAME:
            strcpy(str, MSG_ERENAME);
            break;
        case SDFS_FSELISTDIR:
            strcpy(str, MSG_ELISTDIR);
            break;
        default:
            strcpy(str, MSG_ERROR);
    }
}

/* recursively create directory */
sdfs_err sdfs_rmkdir(const sdfs_str path)
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
    
sdfs_err sdfs_rmdir_r(sdfs_str path, lsdir_mtsafe *mtsafe)
{
    DIR *dd;    
    if (!mtsafe->init) {
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
    if (dd)
        while (1) {
            dir = readdir(dd);
            if (dir) {
                strcat(mtsafe->canonpath, "/");
                strcat(mtsafe->canonpath, dir->d_name);
               
                if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") &&
                    strcmp(dir->d_name, "..")) {
                    if (sdfs_rmdir_r(mtsafe->canonpath, mtsafe) ==
                        SDFS_FSELISTDIR) {
                        closedir(dd);
                        return SDFS_FSELISTDIR;   
                    }
                } else if (dir->d_type != DT_DIR) {
                    printf("deleting %s\n", mtsafe->canonpath);
                    unlink(mtsafe->canonpath);
                }
                mtsafe->canonpath[strlen(mtsafe->canonpath) - 
                    strlen(dir->d_name) - 1] = '\0';
            } else {
                printf("deleting %s\n", mtsafe->canonpath);
                rmdir(mtsafe->canonpath);
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

/* file or directory deletion function */
static sdfs_err sdfs_rment(const sdfs_str path)
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
