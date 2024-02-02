/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef SECLAYER_H
#define SECLAYER_H

#include <sdfs_defs.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/stat.h>

/* security layer error definitions */

#define SDFS_SECSUCCESS SDFS_SUCCESS
#define SDFS_SECERROR SDFS_ERROR + -1
#define SDFS_SECEMEM SDFS_ERROR + -2
#define SDFS_SECELOGIN SDFS_ERROR + -3
#define SDFS_SECECHMOD SDFS_ERROR + -4

/* security layer specific datatypes */

typedef uint64_t sdfs_uid;
typedef mode_t sdfs_mode;

/* obscure structure for user identity */
typedef struct sdfs_id *sdfs_id;

/* initialize security layer */
sdfs_err sdfs_secinit(sdfs_id *id, sdfs_str usrfile);
/* login function */
sdfs_err sdfs_seclogin(sdfs_id id, sdfs_str usr, sdfs_str pwd);
/* logout function */
void sdfs_seclogout(sdfs_id id);
/* finalize security layer */
void sdfs_secfin(sdfs_id id);
/* change file or directory access */
sdfs_err sdfs_secchmod(sdfs_id id, sdfs_str path, sdfs_mode mode);
/* integer error number to string message */
void sdfs_secetomsg(const sdfs_err err, sdfs_str str);

#endif
