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

	if((handle = creat(argv[1], S_IREAD | S_IWRITE)) == -1){
		printf("Can't create destination file !\n");
		exit(-1);
	}
	close(handle);
	if((handle = open(argv[1], O_WRONLY | O_BINARY)) == -1){
		printf("Can't open destination file !\n");
		exit(-1);
	}
	recal();
	for(cyl = 0; cyl < 80; cyl++)
		for(head = 0; head < 2; head++){
			readsec(cyl, head, buffer);
			write(handle, buffer, 512 * 9);
		}
	close(handle);
}
