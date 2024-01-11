#include <stdio.h>
#include <sdfs_fslayer.h>
#include <malloc.h>

int main(void)
{
    
    sdfs_str str = malloc(256);
    int x = sdfs_mkfile("/home/xder");
    sdfs_etomsg(x, str);
    printf("%s\n", str);
    return 0;
}

