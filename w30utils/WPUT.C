#include <string.h>
#include <stdio.h>
#include <math.h>
#include "w30.h"

typedef unsigned long DWORD ;
typedef unsigned int  WORD ;
typedef unsigned int  UINT ;
typedef unsigned char W8SAMPLE ;
typedef short W16SAMPLE ;

struct WavFormat
{
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
    UINT nBitsPerSample ;
} ;

unsigned char outputBuffer[3072*3] ;

main(argc,argv)
int argc ;
char *argv[] ;
{
FILE *fin ;
char chunk[5] ;
int i,j ;
unsigned int channel ;
unsigned long size ;
char GotFmt,GotData ;
unsigned long dataOffset ;
struct WavFormat fmt;
unsigned long dataSize ;
unsigned long srate ;
ToneDirElem Directory[32] ;
int RamA,RamB,position ;
int ram,track,blockCount,sampleSize,outputPos,destTone ;
unsigned long sampleCount ;
char use16Bits ;
W8SAMPLE w8s1,w8s2 ;
W16SAMPLE w16s1,w16s2 ;
unsigned short us1,us2 ;
char *Current ;
ToneParams *tone ;

 /* Check Usage */

    if (argc!=2)
    {
        fprintf (stderr,"\nUsage : %s SoundFile\n\n",argv[0]) ;
        exit(-1) ;
    }

 /* Check Disk */

    if (!CheckDisk())
    {
       fprintf (stderr,"\n\nNot a W30 sound disk in drive A !\n\n") ;
       exit(-1) ;
    }

 /* Try to open SoundFile */

    if ((fin=fopen(argv[1],"rb"))==NULL)
    {
        fprintf (stderr,"\nError opening %s for input\n",argv[1]) ;
        perror("") ;
        exit(-1) ;
    }

 /* Start Reading Sound Header */

    chunk[4]=0 ;

 /* check RIFF chunk */

    fread(chunk,4,1,fin) ;
    if (strcmp(chunk,"RIFF"))
    {
        fprintf (stderr,"\nError, RIFF chunk not found\n\n");
        exit(-1) ;
    }

 /* Skip RIFF size */

    fread(&size,sizeof(DWORD),1,fin) ;

 /* check WAVE type */

    fread(chunk,4,1,fin) ;
    if (strcmp(chunk,"WAVE"))
    {
        fprintf (stderr,"\nError, WAVE chunk not found\n\n");
        exit(-1) ;
    }

 /* Walk through other chunks */

    GotFmt=0 ; GotData=0 ;

    while((!feof(fin))&&((GotFmt==0)||(GotData==0)))
    {

     /* Read The Next Chunk */

        fread(chunk,4,1,fin) ;
        fread(&size,sizeof(DWORD),1,fin) ;

     /* Treat The Chunk */

        if (!strcmp(chunk,"fmt "))
        {
            if (size!=sizeof(struct WavFormat))
            {
                fprintf (stderr,"\nWarning: Size Error In 'fmt ' chunk\n") ;
                fseek (fin,size,SEEK_CUR) ;
            }
            else
            {
                fread(&fmt,size,1,fin) ;
                GotFmt=1 ;
            }
        }
        else if (!strcmp(chunk,"data"))
        {
            dataOffset=ftell(fin) ;
            dataSize=size ;
            GotData=1 ;
        }
        else
        {
            fprintf (stderr,"Skipping Unknown Chunk [%s]\n",chunk) ;
            fseek (fin,size,SEEK_CUR) ;
        }
    }

    if (!GotData)
    {
        fprintf (stderr,"\nError: Couldn't find 'data' chunk !\n\n") ;
        exit(-1) ;
    }
    if (!GotFmt)
    {
        fprintf (stderr,"\nError: Couldn't find valid 'fmt ' chunk !\n\n") ;
        exit(-1) ;
    }

 /* Check Format */

    printf ("\nWindows Sound Report\n") ;
    printf ("--------------------\n") ;
    printf ("Format : %d\n",fmt.wFormatTag) ;
    printf ("nChannels : %d\n",fmt.nChannels) ;
    printf ("nSamplesPerSec : %ld\n",fmt.nSamplesPerSec) ;
    printf ("nAvgBytesPerSec : %ld\n",fmt.nAvgBytesPerSec) ;
    printf ("nBlockAlign : %d\n",fmt.nBlockAlign) ;
    printf ("nBitsPerSamples : %d\n",fmt.nBitsPerSample) ;

    if (fmt.wFormatTag!=1)
    {
        fprintf (stderr,"\nError, Cannot Deal With Format Type %d\n\n",fmt.wFormatTag) ;
        exit(-1) ;
    }


  /* compute sound characteristics */

    srate=fmt.nSamplesPerSec ;
    dataSize/=fmt.nBlockAlign ;
    sampleSize=fmt.nBitsPerSample/8 ;
	use16Bits=(sampleSize==2) ;
	
  /* get disk content */

    LoadSector(1,7,9,(char far *)Directory) ;

  /* compute free ram (a/b) */


    RamA=18 ;
    RamB=18 ;

    Current=(char *)Directory ;  
    
    for (j=0;j<32;j++)
    {
        if (Current[13]!=RAM_UNUSED)
        {
            if (Current[10]==0) // NO sub
            {
                switch (Current[13])
                {
                  case RAM_A:

                    RamA-=Current[15] ;
                    break ;

                  case RAM_B:

                    RamB-=Current[15] ;
                    break ;
                }
            }
        }
        Current+=16 ;
    }

    printf ("Disk Free Ram :    RAMA:%2.2d  RAMB:%2.2d\n",RamA,RamB) ;

	// Computes some characteristics

    blockCount=(int)ceil((double)dataSize/12288.0) ;

    printf ("\nNeed %d blocks per channel\n\n",blockCount) ;

    for (channel=0;channel<fmt.nChannels;channel++)
    {
    
	// Look out for some free RAM

	    if (RamA>=blockCount)
	    {
	    	ram=RAM_A ;
	    	position=18-RamA ;
	        track=8+2*position ;
	        RamA-=blockCount ;
	    }
	    else
	    {
			if(RamB>=blockCount)
		    {
		    	ram=RAM_B ;
		    	position=18-RamB ;
		        track=44+2*position ;
		        RamB-=blockCount ;
		    }
		    else
		    {
		    	printf (" No free Ram left for channel %d\n",channel) ;
		    	fclose(fin) ;
		    	exit(-1) ;
		    }
		}
		
	// Look for free tone
	
	    Current=(char *)Directory ;  

		destTone=-1 ;
		    
	    for (j=0;j<32;j++)
	    {
	        if ((Current[13]==RAM_UNUSED)||(Current[15]==0))
	        {
        		destTone=j ;
        		break ;
        	}
        	Current+=16 ;
        }
        
        if (destTone==-1)
        {
        	printf ("No tone left to store channel %d\n",channel) ;
        	exit(-1) ;
        }
        
		printf ("Storing channel %d in ram %c and tone #%d\n",channel,ram==RAM_A?'A':'B',destTone+1) ;
			
     // Initiate file position
	    		
  		fseek(fin,dataOffset+channel*sampleSize,SEEK_SET) ; 
  		
     // Go on with convertion

		outputPos=0 ;
		sampleCount=dataSize ;
		
		while (sampleCount>0)
		{
			if (use16Bits)
			{
				fread(&w16s1,1,sizeof(W16SAMPLE),fin) ;
				fseek(fin,(fmt.nChannels-1)*sampleSize,SEEK_CUR) ;
				sampleCount-- ;
				
				if (sampleCount>0)
				{
					fread(&w16s2,1,sizeof(W16SAMPLE),fin) ;
					fseek(fin,(fmt.nChannels-1)*sampleSize,SEEK_CUR) ;
					sampleCount-- ;
				}
				else
				{
					w16s2=0 ;
				}
			}
			else
			{
				fread(&w8s1,1,sizeof(W8SAMPLE),fin) ;
				fseek(fin,(fmt.nChannels-1)*sampleSize,SEEK_CUR) ;
				sampleCount-- ;
				
				if (sampleCount>0)
				{
					fread(&w8s2,1,sizeof(W8SAMPLE),fin) ;
					fseek(fin,(fmt.nChannels-1)*sampleSize,SEEK_CUR) ;
					sampleCount-- ;
				}
				else
				{
					w8s2=128 ;
				}
				
			 // converts to 16 bits
			 
				w16s1=(w8s1+128)<<8 ;
				w16s2=(w8s2+128)<<8 ;
				
			}

//			printf ("From 0x%4.4x 0x%4.4x -> ",w16s1,w16s2) ;

			memcpy(&us1,&w16s1,sizeof(W16SAMPLE)) ;
			memcpy(&us2,&w16s2,sizeof(W16SAMPLE)) ;

//			printf (" 0x%4.4x 0x%4.4x -> ",us1,us2) ;

	     // Store in buffer
	                                   
	  		outputBuffer[outputPos++]=us1>>8 ;
//	  		printf ("0x%2.2x ", outputBuffer[outputPos-1]) ;
	  		outputBuffer[outputPos++]=(us1&0x00F0)+((us2&0x00F0)>>4) ;
//	  		printf ("0x%2.2x ", outputBuffer[outputPos-1]) ;
	  		outputBuffer[outputPos++]=us2>>8 ;
//	  		printf ("0x%2.2x\n", outputBuffer[outputPos-1]) ;

         // if one disk track is filled, write it to disk
      	  		
	  		if (outputPos==3072*3)
	  		{
				if (!SaveSectors(0,track,1,18,outputBuffer))
				{
					printf ("Failed to write wave data to disk\n") ;
					exit(-1) ;
				}
				track+=1 ;
				outputPos=0 ;
	  		}
	  	} 
	  	
	  // if buffer needs to be output, do it
	  
	  	if (outputPos!=0)
	  	{
	  		memset(&outputBuffer[outputPos],0,(3072*3-outputPos)) ;
			if (!SaveSectors(0,track,1,18,outputBuffer))
			{
				printf ("Failed to write wave data to disk\n") ;
				exit(-1) ;
			}
		}

		printf ("Updating tone data\n") ;
		    
	  // load tone data
	
		LoadSectors(1,7,1,8,outputBuffer) ;
		
	  // Writes tone data
	  
		tone=(ToneParams *)&outputBuffer[destTone*0x80] ;
		memset(tone,0,0x80) ;
		
	  // Tone name       
	  
		Current=argv[1] ;    
	  	i=0 ;
		while ((*Current!=0)&&(*Current!='.')&&(i<8))
		{
			tone->Name[i++]=*Current++ ;
		}
		if (fmt.nChannels>2)
		{
			if (i>7) i=7 ;
			tone->Name[7]='1'+channel ;
		}
		
	 // Put some parameters to default values

		tone->OutputNumber=channel ;		
		tone->Rate=RATE_30K 	;  	// Rate
		tone->OrgKey=60			;   // C5
		tone->RamNumber=ram		;
		tone->Position=position	;
		tone->Size=blockCount	;
		Put12BitValue(tone->Stop,dataSize) ;
		tone->LoopMode=LOOP_ONESHOT ;
		tone->OutputLevel=127 ;
		tone->TVASustainSegment=1 ;
		tone->TVAReleaseSegment=2 ;
		tone->TVASegments[0].rate=127 ;
		tone->TVASegments[1].rate=127 ;
		tone->TVASegments[2].rate=127 ;
		tone->TVASegments[0].level=127 ;
		tone->TVASegments[1].level=127 ;
		tone->PitchFollow=1 ;	
		
	  // writes tone data
	
		SaveSectors(1,7,1,8,outputBuffer) ;
		  			   
	  // updates directory ( located in the same buffer )
	
		printf ("Updating Directory\n") ;
		  
	  	memcpy(Directory[destTone].Name,tone,16) ;
	
	    SaveSector(1,7,9,(char far *)Directory) ;

    }

	  
}
