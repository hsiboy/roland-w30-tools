#include <stdlib.h>
#include <bios.h>
#include <stdio.h>                         
#include <string.h>

int LoadSector(side,track,sector,buffer)
int side,track,sector ;
char *buffer ;
{
struct _diskinfo_t DI;
unsigned status ;
    
    DI.drive=0 ;			// USES A:
    DI.head=side ;
    DI.track=track ;
    DI.sector=sector ;  	// From 1->9
    DI.nsectors=1 ;
    DI.buffer=(void far*)buffer ;                                   
    
//	printf ("Accessing h:%d t:%d s:%d\n",side,track,sector) ;	

    while ((status=_bios_disk(_DISK_READ,&DI)>>8)==6) ;  // Don't warn for disk change

    if (status!=0)
    {
        printf ("Error 0x%x in LoadSector while trying to access h:%d t:%d s:%d\n",status,side,track,sector) ;
        return(0) ;
    }
    return(1) ;
}


int LoadSectors(side,track,sector,number,buffer)
int side,track,sector,number ;
char *buffer ;
{
int i;

    for (i=0;i<number;i++)
    {
        if (LoadSector(side,track,sector,buffer))
        {
        	buffer+=512 ;
        	sector++ ;
        	if (sector>9)
        	{
            	sector=1 ;
            	side++ ;
            	
            	if (side>2)
            	{   
            		side=0 ;
            		track++ ;
            	}  	
        	}
        }
        else
        {
        	return(0) ;
        }
    }
    return(1) ;
}

int SaveSector(side,track,sector,buffer)
int side,track,sector ;
char *buffer ;
{
struct _diskinfo_t DI;
unsigned status ;
    
    DI.drive=0 ;			// USES A:
    DI.head=side ;
    DI.track=track ;
    DI.sector=sector ;  	// From 1->9
    DI.nsectors=1 ;
    DI.buffer=(void far*)buffer ;                                   
    
//  	printf ("Accessing h:%d t:%d s:%d\n",side,track,sector) ;	

    while ((status=_bios_disk(_DISK_WRITE,&DI)>>8)==6) ;  // Don't warn for disk change

    if (status!=0)
    {
        printf ("Error 0x%x in SaveSector while trying to access h:%d t:%d s:%d\n",status,side,track,sector) ;
        return(0) ;
    }
    return(1) ;
}

int SaveSectors(side,track,sector,number,buffer)
int side,track,sector,number ;
char *buffer ;
{
int i;

    for (i=0;i<number;i++)
    {
        if (SaveSector(side,track,sector,buffer))
        {
        	buffer+=512 ;
        	sector++ ;
        	if (sector>9)
        	{
            	sector=1 ;
            	side++ ;
            	
            	if (side>2)
            	{   
            		side=0 ;
            		track++ ;
            	}  	
        	}
        }
        else
        {
        	return(0) ;
        }
    }
    return(1) ;
}


/*
    - Checks if usable disk
 */
 
int CheckDisk()
{
char Sector[512] ;

// TODO: Add recognition of S-330/S-550 disks

    LoadSectors(0,0,1,1,(char far *)Sector) ;
    Sector[0x2a]=0 ;
    return((int)(strcmp("W-30 Sound",&Sector[0x20])==0)) ;
}

unsigned long Get12BitValue(unsigned char *array)
{
unsigned long us ;

	us=array[0] ;
	us=us*256+array[1] ;
	us=us*256+array[2] ;      
	return(us) ;
}

void Put12BitValue(unsigned char *array,unsigned long us)
{
	array[2]=us&0x000000FF ;
	us>>=8 ;
	array[1]=us&0x000000FF ;
	us>>=8 ;
	array[0]=us&0x000000FF ;
}

