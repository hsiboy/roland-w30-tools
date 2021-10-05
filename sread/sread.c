#include <stdio.h>
#include <fcntl.h>
#include <io.h>

unsigned char b[9 * 512], *ipoi;
unsigned char bout[3456], *opoi;

extern readsec(unsigned, unsigned, void *);
extern recal();

main(argc, argv) int argc;
char *argv[];
{
    int i, j, cnt, time, this;
    int handle, trk, sec, len;

    if ((handle = open(argv[1], O_CREAT | O_WRONLY | O_BINARY)) == -1)
    {
        printf("Can't create file !\n");
        exit(-1);
    }
    sscanf(argv[2], "%d", &trk);
    sscanf(argv[3], "%d", &len);
    recal();
    for (i = trk; i < len; i++)
    {
        printf("track : %.2d\r", i);
        readsec(i, 0, b);
        opoi = bout;
        ipoi = b;
        for (j = 0; j < 1152; j++)
        {
            *opoi++ = ipoi[1];
            *opoi++ = (*ipoi & 0xf0) | (ipoi[2] >> 4);
            *opoi++ = ipoi[3];
            ipoi += 4;
        }
        write(handle, bout, 3456);

        readsec(i, 1, b);
        opoi = bout;
        ipoi = b;
        for (j = 0; j < 1152; j++)
        {
            *opoi++ = ipoi[1];
            *opoi++ = (*ipoi & 0xf0) | (ipoi[2] >> 4);
            *opoi++ = ipoi[3];
            ipoi += 4;
        }
        write(handle, bout, 3456);
    }
    close(handle);
}
