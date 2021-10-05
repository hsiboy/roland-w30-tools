#include <bios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "w30.h"

//
// TODO : make 8 / 16 bits save using flag
//        allow wildcards
//        allow B:
//        port to windows
//	  save only from start to end
//        create chunk with loop information
	      

#define MONO 1
#define STEREO 2 

// #define USE8BIT

typedef unsigned long DWORD ;
typedef unsigned int  WORD ;
typedef unsigned int  UINT ;
#ifdef USE8BIT
typedef unsigned char WSAMPLE ;
#else
typedef short WSAMPLE ;
#endif

struct WavFormat
{
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    UINT nBitsPerSample ;
} ;
ToneDirElem Directory[32] ;

unsigned char inputBuf[12000] ;
WSAMPLE outputBuf[6144] ;

main(argc,argv)
int argc ;
char *argv[] ;
{
int i,t ;
int ToneNum ;
char FileName[16] ;
char chunk[5] ;
long Rate ;
long FirstTrack ;
long TrackNum ;
long Size ;
FILE *soundFileOut ;
struct WavFormat fmt ;
DWORD size ;
WSAMPLE ws1,ws2,*dest ;
unsigned char byte1,byte2,byte3,*src ;

 /* Check usage */

    if (argc!=2)
    {
        printf ("\nUsage: %s ToneName\n",argv[0]) ;
        exit(-1) ;
    }
    if (!CheckDisk())
    {
        printf ("Not A W-30 Disk in drive A:!\n");
        exit(-1) ;
    }
                                                            
	printf ("\n") ;
	                                                            
    strcpy(FileName,argv[1]) ;
    strcat(FileName,".wav") ;

 /* Load Directory */

    LoadSectors(1,7,9,1,(char *)Directory) ;

 /* remove underscore */

    for (i=0;i<(int)strlen(argv[1]);i++)
    {
        if (argv[1][i]=='_') argv[1][i]=' ' ;
    }

 /* Search for the specified sound */

    ToneNum=-1 ;
    for (i=0;i<32;i++)
    {
        Directory[i].padding_byte=0 ;
        if (strnicmp(Directory[i].Name,argv[1],strlen(argv[1]))==0)
        {
            ToneNum=i ;
        }
    }

 /* Exit If Not Found */

    if (ToneNum==-1)
    {
    	printf ("%s Not found !\n\n",argv[1]) ;
		do_wdir() ;  
		exit(-1) ;
    }

  /* Get The Sound Caracteristics */

    printf (" Reading tone #:%d ( %d x 0.4 sec )\n",ToneNum,Directory[ToneNum].Size) ;
    FirstTrack=8+2*Directory[ToneNum].Position+36*Directory[ToneNum].RamNumber ;
    TrackNum=2*Directory[ToneNum].Size ;
    Rate=Directory[ToneNum].Rate==RATE_30K?30000L:15000L ;
    Size=Directory[ToneNum].Size*12288L ;

    printf (" First track:%ld\n Number of tracks to be read:%ld\n Sample rate:%ld\n Number of samples:%ld\n",FirstTrack,TrackNum,Rate,Size) ;

  /* Open the output file */

    soundFileOut=fopen(FileName,"wb") ;
    if (soundFileOut==NULL)
    {
        printf (" Cannot Open %s for output\n",FileName) ;
        exit(-1) ;
    }

  /* Write The Windows WAVE RIFF Headet */

    printf (" Writing Windows %dbit Mono Sound\n",8*sizeof(WSAMPLE)) ;

  /* write RIFF Headr */

    strcpy(chunk,"RIFF") ;
    fwrite(chunk,4,1,soundFileOut) ;
    size=4+(4+sizeof(DWORD))*2+sizeof(struct WavFormat)+Size*sizeof(WSAMPLE) ;
    fwrite(&size,sizeof(DWORD),1,soundFileOut) ;
    strcpy(chunk,"WAVE") ;
    fwrite(chunk,4,1,soundFileOut) ;

 /* write format chunk */

    strcpy(chunk,"fmt ") ;
    fwrite(chunk,4,1,soundFileOut) ;
    size=sizeof(struct WavFormat) ;
    fwrite(&size,sizeof(DWORD),1,soundFileOut) ;

    fmt.wFormatTag=1 ; /* PCM SOUND */
    fmt.nChannels=1;
    fmt.nSamplesPerSec=Rate ;
    fmt.nAvgBytesPerSec=fmt.nChannels*fmt.nSamplesPerSec*sizeof(WSAMPLE) ;
    fmt.nBlockAlign=fmt.nChannels*sizeof(WSAMPLE) ;
    fmt.nBitsPerSample=sizeof(WSAMPLE)*8 ;

    fwrite (&fmt,sizeof(struct WavFormat),1,soundFileOut) ;

 /* convert and write Sound Data */

    strcpy(chunk,"data") ;
    fwrite(chunk,4,1,soundFileOut) ;
    size=sizeof(WSAMPLE)*Size*fmt.nChannels ;
    fwrite(&size,sizeof(DWORD),1,soundFileOut) ;

    for (t=0;t<TrackNum;t++)
    {
        printf (" Converting...(%d %%)\r",((t+1)*100)/TrackNum) ;
        
     /* Get W30 sound chunk */
     
        if (!LoadSectors(0,(int)FirstTrack+t,1,(int)18,(char *)inputBuf))
        {
        	printf (" Aborting...\n")  ;
        	fclose(soundFileOut) ;
        	exit(-1) ;
        }

     /* convert it  */

        src=inputBuf ;
        dest=outputBuf ;

        for (i=0;i<3072;i++)
        {
        	byte1=*src++ ;
        	byte2=*src++ ;
        	byte3=*src++ ;
        	
        	if (sizeof(WSAMPLE)==2)
        	{
        		ws1=(byte1<<8)+(byte2&0xF0) ;
        		ws2=((byte3<<4)+(byte2&0x0F))<<4 ;
        	}
        	else
        	{
        		ws1=byte1-128 ;
        		ws2=byte3-128 ;
        	}
        	
        	*dest++=ws1 ;
        	*dest++=ws2 ;
        }

     /* write to outfile */

        fwrite(outputBuf,6144,sizeof(WSAMPLE),soundFileOut) ;

    }
    printf (" Conversion...Done     \n") ;
    fclose(soundFileOut) ;
	return(0) ;
}
