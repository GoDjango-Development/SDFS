/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef SDFS_DEFS_H
#define SDFS_DEFS_H

#include <inttypes.h>
#include <limits.h>

/* global error definitions */
#define SDFS_SUCCESS 0
#define SDFS_ERROR -1

/* global error messages definitions */
#define SDFS_SUCCESSMSG "sdfs: operation completed"
#define SDFS_ERRORMSG "sdfs: operation failure"

/* sdfs datatypes */
typedef char *sdfs_str;
typedef const char *sdfs_constr;
typedef sdfs_str *sdfs_pstr;
typedef char sdfs_char;
typedef char sdfs_path[PATH_MAX];
typedef int sdfs_err;
typedef int64_t sdfs_int64;
typedef char sdfs_char;
typedef void *sdfs_buf;

#endif
