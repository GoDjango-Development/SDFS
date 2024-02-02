/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#include <sdfs_seclayer.h>
#include <sdfs_util.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SC_INVUSR -1

/* structure definition for user identity */
struct sdfs_id{
    sdfs_char usrfile[PATH_MAX];
    sdfs_char usr[LINE_MAX];
    sdfs_char pwd[LINE_MAX];
    sdfs_uid id;
    sdfs_char inv;
};

/* login recheck */
static sdfs_err sdfs_lrcheck(sdfs_id id);

/* initialize security layer */
sdfs_err sdfs_secinit(sdfs_id *id, sdfs_str usrfile)
{
    *id = malloc(sizeof(sdfs_id));
    if (!*id)
        return SDFS_SECEMEM;
    (*id)->id = SC_INVUSR;
    (*id)->inv = 0;
    strcpy((*id)->usrfile, usrfile);
    return SDFS_SECSUCCESS;
}

/* login function */
sdfs_err sdfs_seclogin(sdfs_id id, sdfs_str usr, sdfs_str pwd)
{
    strcpy(id->usr, usr);
    strcpy(id->pwd, pwd);
    return sdfs_lrcheck(id);
}

/* logout function */
void sdfs_seclogout(sdfs_id id)
{
    id->id = SC_INVUSR;
}

/* finalize security layer */
void sdfs_secfin(sdfs_id id)
{
    free(id);
}

/* login recheck */
static sdfs_err sdfs_lrcheck(sdfs_id id)
{
    int fd = open(id->usrfile, O_RDONLY);
    if (fd == -1)
        return SDFS_SECELOGIN;
    struct stat st;
    if (stat(id->usrfile, &st) == -1) {
        close(fd);
        return SDFS_SECELOGIN;
    }
    void *buf = malloc(st.st_size);
    if (!buf) {
        close(fd);
        return SDFS_SECEMEM;
    }
    if (sdfs_readchunk(fd, buf, st.st_size) == -1) {
        close(fd);
        free(buf);
        return SDFS_SECELOGIN;
    }
    close(fd);
    free(buf);
    return SDFS_SECSUCCESS;
}
