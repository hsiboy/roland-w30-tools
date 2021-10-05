#include <stdio.h>
#include <fcntl.h>
#include <io.h>

unsigned char b[9 * 512];

extern readsec(unsigned, unsigned, void *);
extern recal();

main(argc, argv) int argc;
char *argv[];
{
    int i, j, cnt, time, this;
    int handle, trk, sec, len;

    if ((handle = open(argv[1], O_RDONLY | O_BINARY)) == -1)
    {
        printf("Can't open file !\n");
        exit(-1);
    }
    sscanf(argv[2], "%d", &trk);
    sscanf(argv[3], "%d", &len);
    recal();
    for (i = trk; i < len; i++)
    {
        printf("track : %.2d\r", i);
        if (!read(handle, b, 9 * 512))
            break;
        writesec(i, 0, b);
        if (!read(handle, b, 9 * 512))
            break;
        writesec(i, 1, b);
    }
    putchar('\n');
    close(handle);
}
