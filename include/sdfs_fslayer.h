#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>

/* fs layer error definitions */

#define SDFS_EEXIST 2
#define SDFS_EACCESS 3

sdfs_err sdfs_mkfile(sdfs_str path);
void sdfs_etomsg(sdfs_err err, sdfs_str str);

#endif
