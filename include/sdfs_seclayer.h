/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef SECLAYER_H
#define SECLAYER_H

#include <sdfs_defs.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/stat.h>

/* security layer error definitions */

#define SDFS_SECSUCCESS 0
#define SDFS_SECERROR -1
#define SDFS_SECEMEM -2
#define SDFS_SECELOGIN -3
#define SDFS_SECECHMOD -4

/* security layer operation enumeration */
enum sdfs_secop {
    SDFS_SECOP_MKFILE
};

/* security layer specific datatypes */

typedef uint64_t sdfs_uid;
typedef mode_t sdfs_mode;
typedef enum sdfs_secop sdfs_secop;

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
/* execute fs layer operation */
sdfs_err sdfs_runop(sdfs_id id, sdfs_secop op, sdfs_buf buf);
/* get fs layer last error */
sdfs_err sdfs_getfserr(sdfs_id id);
/* get fs layer last error message */
sdfs_str sdfs_getfsmsg(sdfs_id id);
/* integer error number to string message */
void sdfs_secetomsg(const sdfs_err err, const sdfs_pstr str);

#endif
