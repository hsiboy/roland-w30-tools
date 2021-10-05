#include <stdio.h>

unsigned char b[9 * 512];
unsigned char *buffer = &b[8 * 512];

extern readsec(unsigned, unsigned, void *);
extern recal();

main()
{
int	i, j, cnt, time, this;
int	x;

	x = 0;
	do{
		recal();
		cnt = 0;
		time = 0;
		while(readsec(7, 1, b)){
			fprintf(stderr, "Can't read !\n");
			getchar();
		}
		putc(0x07, stderr);
		printf("Disk no.: %.3d\n\n", ++x);
		for(i = 0; i < 32; i++){
			if(buffer[15 + i * 16] != 0){
				cnt++;
				if(buffer[10 + i * 16] == 0){
					time += (this = buffer[15 + i * 16]);
					this *= 4;
				}
				for(j = i * 16; j < 8 + i * 16; j++)
					putchar(buffer[j]);
				if(buffer[10 + i * 16] == 0)
					printf(" %d.%d", this / 10, this % 10);
				else
					printf(" Sub");
				if(cnt < 5)
					putchar('\t');
				else{
					cnt = 0;
					putchar('\n');
				}
			}
		}
		printf("\n\nFree time : %d.%d\n\n", ((36 - time) * 4) / 10, ((36 - time) * 4) % 10);
		printf("--------------------------------------------------------------------------------\n");
	}while(getch() != 'q');
}
