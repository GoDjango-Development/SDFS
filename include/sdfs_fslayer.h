/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>
#include <sys/stat.h>
#include <dirent.h>

/* fs layer error definitions */

#define SDFS_FSSUCCESS 0
#define SDFS_FSERROR -1
#define SDFS_FSEEXIST -2
#define SDFS_FSEACCESS -3
#define SDFS_FSENOENT -4
#define SDFS_FSEIO -5
#define SDFS_FSERENAME -6
#define SDFS_FSELISTDIR -7
#define SDFS_FSERMDIR -8

/* fs layer enumerations */

enum sdfs_clbkctrl { 
    SDFS_CLBKCTRL_STOP, SDFS_CLBKCTRL_CONT
};

/* fs layer strcutres */

struct lsdir_mtsafe {
    char canonpath[PATH_MAX];
    int init;
    sdfs_err err;
};

/* fs layer specific datatypes */

typedef struct stat sdfs_stat;
typedef enum sdfs_clbkctrl sdfs_clbkctrl;
typedef struct lsdir_mtsafe lsdir_mtsafe;

/* fs layer callbacks prototypes */

/* list directory function callback. each call represents a new entry in the
 * directory. dir pointer will be null in the last callback invokation */
typedef void (*sdfs_lsdir_clbk)(const sdfs_str ent, sdfs_clbkctrl *ctrl);

/* file creation function */
sdfs_err sdfs_mkfile(const sdfs_str path);
/* directory creation function */
sdfs_err sdfs_mkdir(const sdfs_str path);
/* file deletion function */
sdfs_err sdfs_rmfile(const sdfs_str path);
/* directory deletion function */
sdfs_err sdfs_rmdir(const sdfs_str path);
/* read byte block from a file */
sdfs_int64 sdfs_readblk(const sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len);
/* write byte block to a file */
sdfs_int64 sdfs_writeblk(const sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len);
/* get file or directory statistic */
sdfs_err sdfs_getstat(const sdfs_str path, sdfs_stat *stat_obj);
/* rename file or directory from oldname to newname */
sdfs_err sdfs_rename(const sdfs_str old_path, const sdfs_str new_path);
/* list directory function */
sdfs_err sdfs_listdir(const sdfs_str path, sdfs_lsdir_clbk callback);
/* recursively directory list function */
sdfs_err sdfs_listdir_r(sdfs_str path, lsdir_mtsafe *mtsafe, 
    sdfs_lsdir_clbk callback);
/* recursively create directory */
sdfs_err sdfs_rmkdir(const sdfs_str path);
/* recursively try to remove directories */
sdfs_err sdfs_rmdir_r(sdfs_str path, lsdir_mtsafe *mtsafe);
/* integer error number to string message */
void sdfs_fsetomsg(const sdfs_err err, const sdfs_pstr str);

#endif
