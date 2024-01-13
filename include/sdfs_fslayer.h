#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>

/* fs layer error definitions */

#define SDFS_EEXIST 2
#define SDFS_EACCESS 3
#define SDFS_ENOENT 4

/* file creation function */
sdfs_err sdfs_mkfile(sdfs_str path);
/* file deletion function */
sdfs_err sdfs_rmfile(sdfs_str path);
/* integer error number to string message */
void sdfs_etomsg(sdfs_err err, sdfs_str str);
/* directory creation function */
sdfs_err sdfs_mkdir(sdfs_str path);

#endif
