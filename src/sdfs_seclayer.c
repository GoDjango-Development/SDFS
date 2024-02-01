#include <sdfs_seclayer.h>
#include <malloc.h>
#include <string.h>

#define SC_INVUSR -1

/* structure definition for user identity */
struct sdfs_id{
    sdfs_char usrfile[PATH_MAX];
    sdfs_char usr[LINE_MAX];
    sdfs_uid id;
    sdfs_char inv;
};

/* initialize security layer */
sdfs_err init_seclayer(sdfs_id *id, sdfs_str usrfile, sdfs_str usr)
{
    *id = malloc(sizeof(sdfs_id));
    if (!*id)
        return SDFS_SECMEM;
    (*id)->id = SC_INVUSR;
    (*id)->inv = 0;
    strcpy((*id)->usrfile, usrfile);
    strcpy((*id)->usr, usr);
    return SDFS_SECSUCCESS;
}

/* finalize security layer */
void fin_seclayer(sdfs_id id)
{
    free(id);
}
