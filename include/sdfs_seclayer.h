#ifndef SECLAYER_H
#define SECLAYER_H

#include <sdfs_defs.h>
#include <limits.h>
#include <inttypes.h>

/* security layer error definitions */

#define SDFS_SECSUCCESS SDFS_SUCCESS
#define SDFS_SECERROR SDFS_ERROR + -1
#define SDFS_SECEMEM SDFS_ERROR + -2
#define SDFS_SECEUSR SDFS_ERROR + -3

/* security layer specific datatypes */

typedef uint64_t sdfs_uid;

/* obscure structure for user identity */
typedef struct sdfs_id *sdfs_id;

/* initialize security layer */
sdfs_err sdfs_secinit(sdfs_id *id, sdfs_str usrfile);
/* login function */
sdfs_err sdfs_seclogin(sdfs_id id, sdfs_str usr, sdfs_str pwd);
/* finalize security layer */
void sdfs_secfin(sdfs_id id);

#endif
