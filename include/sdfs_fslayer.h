#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>

/* fs layer error definitions */

#define SDFS_FSSUCCESS SDFS_SUCCESS
#define SDFS_FSERROR SDFS_ERROR + -1
#define SDFS_FSEEXIST SDFS_ERROR + -2
#define SDFS_FSEACCESS SDFS_ERROR + -3
#define SDFS_FSENOENT SDFS_ERROR + -4
#define SDFS_FSEIO SDFS_ERROR + -5

/* file creation function */
sdfs_err sdfs_mkfile(sdfs_str path);
/* directory creation function */
sdfs_err sdfs_mkdir(sdfs_str path);
/* file deletion function */
sdfs_err sdfs_rmfile(sdfs_str path);
/* directory deletion function */
sdfs_err sdfs_rmdir(sdfs_str path);
/* read byte block from a file */
sdfs_int64 sdfs_readblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len);
/* write byte block to a file */
sdfs_int64 sdfs_writeblk(sdfs_str path, sdfs_buf buf, sdfs_int64 offset,
    sdfs_int64 len);
/* integer error number to string message */
void sdfs_etomsg(sdfs_err err, sdfs_str str);

#endif
