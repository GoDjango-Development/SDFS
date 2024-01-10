#ifndef FSLAYER_H
#define FSLAYER_H

#include <sdfs_defs.h>

/* FS Layer error definitions */

#define SDFS_FSSUCCESS 0
#define SDFS_FSEEXIST 1

void test(void);
int sdfs_fsmkfile(sdfs_str path);
void sdfs_fserrtomsg(sdfs_err err, sdfs_str str);

#endif
