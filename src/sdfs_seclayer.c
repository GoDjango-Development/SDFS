/* programmer: luis miguel
 * email: lmdelbahia@gmail.com */

#include <sdfs_seclayer.h>
#include <sdfs_fslayer.h>
#include <sdfs_util.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>

/* error messages definitions */
#define MSG_SUCCESS "security layer: operation completed"
#define MSG_EMEM "security layer: no memory available"
#define MSG_ELOGIN "security layer: user cannot be loged-in"
#define MSG_ERROR "security layer: operation failure"
#define MSG_EOP "security layer: invalid specified operation"

#define SC_INVUSR -1
#define SC_USRSEP ":"
#define SC_USREND "\n"

/* structure definition for user identity */
struct sdfs_secid {
    sdfs_char usrfile[PATH_MAX];
    sdfs_char usr[LINE_MAX];
    sdfs_char pwd[LINE_MAX];
    sdfs_secuid id;
    sdfs_secuid gid;
    sdfs_char inv;
    sdfs_err fserr;
    sdfs_str fsmsg;
};

/* login recheck */
static sdfs_err sdfs_lrcheck(sdfs_secid id);

/* initialize security layer */
sdfs_err sdfs_secinit(sdfs_secid *id, sdfs_str usrfile)
{
    *id = malloc(sizeof(sdfs_secid));
    if (!*id)
        return SDFS_SECEMEM;
    (*id)->id = SC_INVUSR;
    (*id)->inv = 0;
    strcpy((*id)->usrfile, usrfile);
    return SDFS_SECSUCCESS;
}

/* login function */
sdfs_err sdfs_seclogin(sdfs_secid id, sdfs_str usr, sdfs_str pwd)
{
    strcpy(id->usr, usr);
    strcpy(id->pwd, pwd);
    return sdfs_lrcheck(id);
}

/* logout function */
void sdfs_seclogout(sdfs_secid id)
{
    id->id = SC_INVUSR;
}

/* finalize security layer */
void sdfs_secfin(sdfs_secid id)
{
    free(id);
}

/* login recheck */
static sdfs_err sdfs_lrcheck(sdfs_secid id)
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
    /* check id->usr user existency the id->usrfile file */
    char tmp[LINE_MAX];
    strcpy(tmp, SC_USRSEP);
    strcat(tmp, id->usr);
    strcat(tmp, SC_USRSEP);
    char *pt = strstr(buf, tmp);
    if (!pt) {
        free(buf);
        return SDFS_SECELOGIN;
    }    
    /* check password matching for the user in the id->usrfile */
    pt = strtok(pt, SC_USREND);
    if (!pt) {
        free(buf);
        return SDFS_SECELOGIN;
    }
    pt = strstr(pt + 1, SC_USRSEP);
    if (!pt) {
        free(buf);
        return SDFS_SECELOGIN;
    }
    pt = strtok(pt + 1, SC_USREND);
    if (strcmp(pt, id->pwd)) {
        free(buf);
        return SDFS_SECELOGIN;
    }
    free(buf);
    /* check id->usr user existency in the unix system database */
    char *ptmp = strtok(tmp + 1, SC_USRSEP);
    struct passwd *pswd = getpwnam(ptmp);
    if (!pswd)
        return SDFS_SECELOGIN;
    /* set to id->id and id->gid the effecive procces ids */
    if (setegid(pswd->pw_gid) == -1)
        return SDFS_SECELOGIN;
    if (seteuid(pswd->pw_uid) == -1)
        return SDFS_SECELOGIN;
    id->id = pswd->pw_uid;
    id->gid = pswd->pw_gid;
    return SDFS_SECSUCCESS;
}

/* change file or directory access */
sdfs_err sdfs_secchmod(sdfs_secid id, sdfs_str path, sdfs_secmode mode)
{
    mode_t oldm = umask(0);
    if (!id || id->id == SC_INVUSR)
        return SDFS_SECECHMOD;
    if (chmod(path, mode) == -1)
        return SDFS_SECECHMOD;
    umask(oldm);
    return SDFS_SECSUCCESS;
}

/* execute fs layer operation */
sdfs_err sdfs_secrunop(sdfs_secid id, sdfs_secop op, sdfs_buf buf)
{
    if (!id || !buf || id->id == SC_INVUSR)
        return SDFS_SECELOGIN;
    switch (op) {
        case SDFS_SECOP_MKFILE:
            if ((id->fserr = sdfs_fsmkfile(buf)) != SDFS_FSSUCCESS) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            }
            break;
        case SDFS_SECOP_MKDIR:
            if ((id->fserr = sdfs_fsmkdir(buf)) != SDFS_FSSUCCESS) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            }
            break;
        case SDFS_SECOP_RMFILE:
            if ((id->fserr = sdfs_fsrmfile(buf)) != SDFS_FSSUCCESS) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            }
            break;
        case SDFS_SECOP_RMDIR:
            if ((id->fserr = sdfs_fsrmdir(buf)) != SDFS_FSSUCCESS) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            }
            break;
        case SDFS_SECOP_READBLK:;
            sdfs_secblkop *blk = buf;
            if ((id->fserr = sdfs_fsreadblk(blk->path, blk->buf, blk->offset, 
                blk->len)) <= SDFS_ERROR) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            } else {
                id->fserr = SDFS_FSSUCCESS;
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECSUCCESS;
            }
            break;
        case SDFS_SECOP_WRITEBLK:
            blk = buf;
            if ((id->fserr = sdfs_fswriteblk(blk->path, blk->buf, blk->offset, 
                blk->len)) <= SDFS_ERROR) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            } else {
                id->fserr = SDFS_FSSUCCESS;
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECSUCCESS;
            }
            break;
        case SDFS_SECOP_STAT:;
            sdfs_secstat *st = buf;
            if ((id->fserr = sdfs_fsgetstat(st->path, &st->stat)) != SDFS_FSSUCCESS) {
                sdfs_fsetomsg(id->fserr, &id->fsmsg);
                return SDFS_SECERROR;
            }
            break;
        default:
            id->fserr = SDFS_FSERROR;
            sdfs_fsetomsg(id->fserr, &id->fsmsg);
            return SDFS_SECEOP;
    }
    sdfs_fsetomsg(id->fserr, &id->fsmsg);
    return SDFS_SECSUCCESS;
}

/* get fs layer last error */
sdfs_err sdfs_secfserr(sdfs_secid id)
{
    if (!id)
        return SDFS_SECERROR;
    return id->fserr;
}

/* get fs layer last error message */
sdfs_str sdfs_secfsmsg(sdfs_secid id)
{
    if (!id)
        return NULL;
    return id->fsmsg;
}

/* integer error number to string message */
void sdfs_secetomsg(const sdfs_err err, const sdfs_pstr str)
{
    switch (err) {
        case SDFS_SECSUCCESS:
            *str = MSG_SUCCESS;
            break;
        case SDFS_SECEMEM:
            *str = MSG_EMEM;
            break;
        case SDFS_SECELOGIN:
            *str = MSG_ELOGIN;
            break;
        case SDFS_SECEOP:
            *str = MSG_EOP;
            break;
        default:
            *str = MSG_ERROR;
    }
}
