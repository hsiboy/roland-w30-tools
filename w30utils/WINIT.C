#include <bios.h>
#include <stdio.h>
#include "w30.h"

char buf[0x200] ;

char WriteSector(side,track,sector,buffer)
int side,track,sector ;
char far *buffer ;
{
struct _diskinfo_t DI;
unsigned status ;


    DI.drive=0 ;
    DI.head=side ;
    DI.track=track ;
    DI.sector=sector ;
    DI.nsectors=1 ;
    DI.buffer=(void far *)buffer ;

    while ((status=_bios_disk(_DISK_WRITE,&DI)>>8)==6) ;

    if (status!=0)
    {
        printf ("Error 0x%x in WriteSector while trying to access h:%d t:%d s:%d\n",status,side,track,sector) ;
        memset((char *)buffer,0,512) ;
        return(0) ;
    }
    return(1) ;
}


void WriteSectors(side,track,sector,number,buffer)
int side,track,sector,number ;
char far *buffer ;
{
int i;

    for (i=0;i<number;i++)
    {
        WriteSector(side,track,sector,buffer) ;
        buffer+=512 ;
        sector++ ;
        if (sector>9)
        {
            side++ ;
            sector=1 ;
            if (side>1)
            {
                track++ ;
                side=0 ;
            }
        }
    }
}

void main()
{
int i ;
ToneDirElem Directory[32] ;
char c ;

    printf ("\nAre you sure you want to init a W30 disk ? (Y/N)") ;
    c=getchar() ;

    if ((c!='Y')&&(c!='y')) exit(-1) ;

  /* write W30 header */

    for (i=0;i<0x80;i++)
    {
        buf[i]=DiskHeader[i] ;
    }
    for (i=0x80;i<0x200;i++)
    {
        buf[i]=0 ;
    }

    WriteSector(0,0,1,(char far *)buf) ;

  /* clear directory */

    for (i=0;i<32;i++)
    {
        strcpy(Directory[i].Name,"        ") ;
        Directory[i].RamNumber=RAM_UNUSED ;
    }

    WriteSectors(1,7,9,1,(char far *)Directory) ;

}

