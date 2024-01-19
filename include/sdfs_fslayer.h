/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>
#include <sys/stat.h>
#include <dirent.h>

/* fs layer error definitions */

#define SDFS_FSSUCCESS SDFS_SUCCESS
#define SDFS_FSERROR SDFS_ERROR + -1
#define SDFS_FSEEXIST SDFS_ERROR + -2
#define SDFS_FSEACCESS SDFS_ERROR + -3
#define SDFS_FSENOENT SDFS_ERROR + -4
#define SDFS_FSEIO SDFS_ERROR + -5
#define SDFS_FSERENAME SDFS_ERROR + -6
#define SDFS_FSELISTDIR SDFS_ERROR + -7

/* fs layer specific datatypes */

typedef struct stat sdfs_stat;
typedef struct dirent *sdfs_dirst;

/* fs layer callbacks prototypes */

/* list directory function callback. each call represents a new entry in the
 * directory. dir pointer will be null in the last callback invokation */
typedef void (*sdfs_lsdir_clbk)(sdfs_dirst dir);

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
/* integer error number to string message */
void sdfs_etomsg(const sdfs_err err, sdfs_str str);

#endif
