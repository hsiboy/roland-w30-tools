#include <stdio.h>
#include <io.h>
#include <fcntl.h>

unsigned char buffer[512 * 9];

extern readsec(unsigned, unsigned, void *);
extern recal();

main(argc, argv)
int	argc;
char	*argv[];
{
unsigned	cyl, sec, head;
int	handle;

	if((handle = open(argv[1], O_RDONLY | O_BINARY)) == -1){
		printf("Can't open source file !\n");
		exit(-1);
	}
	recal();
	for(head = 0; head < 2; head++){
		read(handle, buffer, 512 * 9);
		if(writesec(1, head, buffer)){
			printf("Error during writing !\n");
			exit(-1);
		}
	}
	close(handle);
	printf("Success !\n");
}
