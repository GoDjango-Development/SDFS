#ifndef SECLAYER_H
#define SECLAYER_H

#include <sdfs_defs.h>
#include <limits.h>
#include <inttypes.h>

/* security layer error definitions */

#define SDFS_SECSUCCESS SDFS_SUCCESS
#define SDFS_SECERROR SDFS_ERROR + -1
#define SDFS_SECMEM SDFS_ERROR + -2

/* security layer specific datatypes */

typedef uint64_t sdfs_uid;

/* obscure structure for user identity */
typedef struct sdfs_id *sdfs_id;

/* initialize security layer */
sdfs_err init_seclayer(sdfs_id *id, sdfs_str usrfile, sdfs_str usr);
/* finalize security layer */
void fin_seclayer(sdfs_id id);

#endif
