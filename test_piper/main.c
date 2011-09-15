#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


int main()
{
    int i;

    setvbuf(stdout,(char*)NULL,_IONBF,0);
    printf("Hello world!\n");

    for(i = 0; i < 5; i ++ )
    {

        fd_set rfds;
        struct timeval tv;
        int retval;

        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);

        /* Wait up to five seconds. */
        tv.tv_sec = 0;
        tv.tv_usec = 2000;

        retval = select(1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

        if (retval == -1)
            perror("select()");
        else if (retval)
        {

            char c;
            read(0, &c, 1);
            fprintf(stderr, "read char: %c\n", c);
        }


        fprintf( i%2 ? stdout : stderr,"i: %d %s\n", i, i%2 ? "stdout" : "stderr");
        usleep(1000);
    }

    return 0;
}
