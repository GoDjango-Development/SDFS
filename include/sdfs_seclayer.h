/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#ifndef SECLAYER_H
#define SECLAYER_H

#include <sdfs_defs.h>
#include <sdfs_fslayer.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/stat.h>

/* security layer error definitions */

#define SDFS_SECSUCCESS 0
#define SDFS_SECERROR -1
#define SDFS_SECEMEM -2
#define SDFS_SECELOGIN -3
#define SDFS_SECECHMOD -4
#define SDFS_SECEOP -5

/* security layer operation enumeration */
enum sdfs_secop {
    SDFS_SECOP_MKFILE, SDFS_SECOP_MKDIR, SDFS_SECOP_RMFILE, SDFS_SECOP_RMDIR,
    SDFS_SECOP_READBLK, SDFS_SECOP_WRITEBLK, SDFS_SECOP_STAT, SDFS_SECOP_RENAME,
    SDFS_SECOP_LSDIR, SDFS_SECOP_LSDIR_R, SDFS_SECOP_MKDIR_R
};

/* data structures for security layer operations */

/* data structure for read and write blocks to files */
struct sdfs_secblkop {
    sdfs_path path;
    sdfs_buf buf;
    sdfs_int64 offset;
    sdfs_int64 len;
};

/* data structure for obtain statistic from a file or directory */
struct sdfs_secstat {
    sdfs_path path;
    sdfs_fsstat stat;
};

/* data structure for renaming a file or directory */
struct sdfs_secrename {
    sdfs_path oldpath;
    sdfs_path newpath;
};

/* data structure for listing directory */
struct sdfs_seclsdir {
    sdfs_path path;
    sdfs_lsdir_clbk callback;
    sdfs_fslsmtsafe mtsafe;
};

/* security layer specific datatypes */

typedef uint64_t sdfs_secuid;
typedef mode_t sdfs_secmode;
typedef enum sdfs_secop sdfs_secop;
typedef time_t sdfs_time;

/* security layer datatype definitions */

/* obscure structure for user identity */
typedef struct sdfs_secid *sdfs_secid;

/* structure typedefs for security layer operations */
typedef struct sdfs_secblkop sdfs_secblkop;
typedef struct sdfs_secstat sdfs_secstat;
typedef struct sdfs_secrename sdfs_secrename;
typedef struct sdfs_seclsdir sdfs_seclsdir;

/* initialize security layer */
sdfs_err sdfs_secinit(sdfs_secid *id, sdfs_str usrfile);
/* login function */
sdfs_err sdfs_seclogin(sdfs_secid id, sdfs_str usr, sdfs_str pwd);
/* logout function */
void sdfs_seclogout(sdfs_secid id);
/* finalize security layer */
void sdfs_secfin(sdfs_secid id);
/* change file or directory access */
sdfs_err sdfs_secchmod(sdfs_secid id, sdfs_str path, sdfs_secmode mode);
/* execute fs layer operation */
sdfs_err sdfs_secrunop(sdfs_secid id, sdfs_secop op, sdfs_buf buf);
/* get fs layer last error */
sdfs_err sdfs_secfserr(sdfs_secid id);
/* get fs layer last error message */
sdfs_str sdfs_secfsmsg(sdfs_secid id);
/* integer error number to string message */
void sdfs_secetomsg(const sdfs_err err, const sdfs_pstr str);

#endif
