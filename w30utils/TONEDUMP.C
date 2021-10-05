#include <stdio.h>
#include "w30.h"

ToneParams *tone ;
char buffer[512*9] ;

int main(int argc,char *argv[])
{
int i,j ;
unsigned char *c ;
int toneNum ;

// check usage

	if (argc!=2)
	{
		printf ("Usage : %s tone_num ( 1->32 )\n",argv[0]) ;
		exit(-1) ;
	}
	
// check disk

	if (!CheckDisk())
	{
		printf ("Wrong disk type !\n") ;
		exit(-1) ;
	}

	sscanf(argv[1],"%d",&toneNum) ;
	
	if (toneNum<1 || toneNum>32)
	{
		printf ("Bad tone number ( %d )\n",toneNum) ;
		exit(-1) ;
	}

// load tone part

	LoadSectors(1,7,1,9,buffer) ;
	tone=(ToneParams *)&buffer[(toneNum-1)*0x80] ;

// dump tone

	printf ("\nTone report:\n") ;
	printf ("------------\n\n") ;
	
// do hex dump

	c=(char *)tone ;
	
	for (i=0;i<0x80;i+=0x10)
	{
		printf ("%2.2x : ",i) ;
		for (j=0;j<0x10;j++)
		{
			printf ("%2.2x ",c[i+j]) ;
		}
		printf ("\n") ;
	}
	
	printf ("\n\n") ;

// do interpreted dump
	
	tone->padding_byte=0 ;
	printf ("Name:\t\t\t\"%s\"\n",tone->Name) ;
	printf ("Subtone Number :\t%d\n",tone->SubToneNumber) ;
	printf ("Sub Type :\t\t%d\n",tone->SubType) ;
	printf ("Sample Rate :\t\t%s\n",tone->Rate==RATE_30K?"30000":"15000") ;
	printf ("Original Key :\t\t%d\n",tone->OrgKey) ;
	printf ("RAM Number :\t\t%s\n",tone->RamNumber==RAM_A?"A":"B") ;
	printf ("Wave Block Position :\t%d\n",tone->Position) ;
	printf ("Wave Block Size :\t%d\n",tone->Size) ;

	printf ("Start Point :\t\t%d\n",Get12BitValue(tone->Start)) ;
	printf ("End Point :\t\t%d\n",Get12BitValue(tone->Stop)) ;
	printf ("Loop Point :\t\t%d\n",Get12BitValue(tone->Loop)) ;
	printf ("Loop Mode :\t\t%d\n",tone->LoopMode) ;       
 	printf ("Output Level :\t\t%d\n",tone->OutputLevel) ;
 	printf ("Pitch Follow :\t\t%s\n",tone->PitchFollow==1?"On":"Off") ;
	printf ("\n") ;

	printf ("TVA Sustain segment :\t%d\n",tone->TVASustainSegment+1) ;
	printf ("TVA Release segment :\t%d\n",tone->TVAReleaseSegment+1) ;
	printf ("TVA Rates :\t\t") ;
	for (i=0;i<8;i++) printf("%3d ",tone->TVASegments[i].rate) ;
	printf ("\nTVA Levels :\t\t") ;
	for (i=0;i<8;i++) printf("%3d ",tone->TVASegments[i].level) ;
	printf ("\n") ;
	printf ("\n") ;

	printf ("TVF Sustain segment :\t%d\n",tone->TVFSustainSegment+1) ;
	printf ("TVF Release segment :\t%d\n",tone->TVFReleaseSegment+1) ;
	printf ("TVF Rates :\t\t") ;
	for (i=0;i<8;i++) printf("%3d ",tone->TVFSegments[i].rate) ;
	printf ("\nTVF Levels :\t\t") ;
	for (i=0;i<8;i++) printf("%3d ",tone->TVFSegments[i].level) ;
	printf ("\n") ;

	return 1 ;
}
