#include <sdfs_fslayer.h>
#include <string.h>

#define SDFS_FSSUCCESSMSG "FS Layer: Operation succeeded."
#define SDFS_FSEEXISTMSG "FS Layer: File already exists."

void test(void)
{
    sdfs_str str = "asdad";
    printf("%s\n", str);
}

int sdfs_fsmkfile(sdfs_str path)
{
    return 0;
}

void sdfs_fserrtomsg(sdfs_err err, sdfs_str str)
{
    switch (err) {
        case SDFS_FSSUCCESS:
            strcpy(str, SDFS_FSSUCCESSMSG);
            break;
        case SDFS_FSEEXIST:
            strcpy(str, SDFS_FSEEXISTMSG);
            break;
    }
}
