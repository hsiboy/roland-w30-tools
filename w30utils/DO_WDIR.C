#include <stdlib.h>
#include <bios.h>
#include <stdio.h>                         
#include <string.h>

#include "w30.h"

char Sector[512] ;

int do_wdir()
{
int i,j ;
char *Current ;
int RamA,RamB ;

    LoadSectors(1,7,9,1,(char far *)Sector) ;
    printf ("\nDirectory of W30 disk is :\n\n") ;
    Current=Sector ;
    RamA=18 ;
    RamB=18 ;
    for (j=0;j<32;j++)
    {
        for (i=0;i<8;i++)
            if (Current[i]==' ') Current[i]='_' ;
        Current[8]=0 ;
        if (Current[13]!=RAM_UNUSED)
        {
            printf ("  %8s : %7ld\n",Current,(long)Current[15]*12288) ;
            if (Current[10]==0)
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
    printf ("\n\n  Free Ram A : %7ld ( %4.1f sec ) \n",(long)RamA*12288,0.4*RamA) ;
    printf ("  Free Ram B : %7ld ( %4.1f sec )\n",(long)RamB*12288,0.4*RamB) ;
    return(1) ;
}
