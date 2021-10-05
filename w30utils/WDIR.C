#include <stdlib.h>
#include <bios.h>
#include <stdio.h>                         
#include <string.h>

main()
{
    if (!CheckDisk())
    {
        printf ("Not A W-30 Disk in Drive A:!\n");
        exit(-1) ;
    }

    do_wdir() ;

    return(1) ;
}
