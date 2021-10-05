#include <stdio.h>
#include <io.h>
#include <fcntl.h>

unsigned char buffer[512 * 9];

extern writesec(unsigned, unsigned, void *);
extern recal();

main(argc, argv)
int	argc;
char	*argv[];
{
unsigned	cyl, sec, head;
int	handle;

	if((handle = open(argv[1], O_BINARY)) == -1){
		printf("Can't open source file !\n");
		exit(-1);
	}
	recal();
	for(cyl = 0; cyl < 80; cyl++)
		for(head = 0; head < 2; head++){
			if(read(handle, buffer, 9 * 512) != 9 * 512){
				printf("Error while reading\n");
				exit(-1);
			}
			writesec(cyl, head, buffer);
		}
	close(handle);
}
