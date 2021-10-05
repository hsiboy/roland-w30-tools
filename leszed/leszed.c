#include <stdio.h>
#include <fcntl.h>
#include <io.h>

unsigned char b[9 * 512];

extern readsec(unsigned, unsigned, void *);
extern recal();

main(argc, argv)
int	argc;
char	*argv[];
{
int	i, j, cnt, time, this;
int	handle, trk, sec, len;

	if((handle = open(argv[1], O_CREAT | O_WRONLY | O_BINARY)) == -1){
		printf("Can't create file !\n");
		exit(-1);
	}
	sscanf(argv[2], "%d", &trk);
	sscanf(argv[3], "%d", &len);
	recal();
	for(i = trk; i < len; i++){
		readsec(i, 0, b);
		write(handle, b, 9*512);
		readsec(i, 1, b);
		write(handle, b, 9*512);
	}
	close(handle);
}
