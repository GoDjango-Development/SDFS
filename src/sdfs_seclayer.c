#include <sdfs_seclayer.h>
#include <malloc.h>
#include <string.h>

#define SC_INVUSR -1

/* structure definition for user identity */
struct sdfs_id{
    sdfs_char usrfile[PATH_MAX];
    sdfs_char usr[LINE_MAX];
    sdfs_char pwd[LINE_MAX];
    sdfs_uid id;
    sdfs_char inv;
};

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
    return SDFS_SECSUCCESS;
}

/* finalize security layer */
void sdfs_secfin(sdfs_id id)
{
    free(id);
}
