/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef SDFS_DEFS_H
#define SDFS_DEFS_H

#include <inttypes.h>

/* global error definitions */
#define SDFS_SUCCESS 0
#define SDFS_ERROR -1

/* global error messages definitions */
#define SDFS_SUCCESSMSG "sdfs: operation completed"
#define SDFS_ERRORMSG "sdfs: operation failure"

/* sdfs datatypes */
typedef char *sdfs_str;
typedef char sdfs_char;
typedef int sdfs_err;
typedef int64_t sdfs_int64;
typedef char sdfs_char;
typedef void *sdfs_buf;

#endif
