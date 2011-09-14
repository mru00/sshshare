#include <pty.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "scp.hxx"


// http://cwshep.blogspot.com/2009/06/showing-scp-progress-using-zenity.html

int get_file(const char* filename)
{
    int pid;
    int status;
    int read_fd;

    char* argv[] = {"/usr/bin/scp", "-B", "-v", "mru@sisyphus.teil.cc:pulltester-trac.tar.gz", ".", NULL};

    switch(pid = forkpty(&read_fd, NULL, NULL, NULL))
    {
    case -1: // ERROR!
        printf("Error with code %i\n", errno);
        return errno;
    case 0: // Child
    {
        status = execv(argv[0], argv);
        break;
    }
    default:
    {
        FILE* f = fdopen(read_fd, "r");
        char c, c1, c2;
        int progress = 0;
//reading character by character isn't horribly efficient...
        while((c = fgetc(f)) != -1)
        {
            if (c == '%')
            {
                if (c1 == ' ')
                    progress = c2 - '0';
                else if (c1 == '0' && c2 == '0')
                    progress = 100;
                else
                    progress = (c1-'0')*10+(c2-'0');

                printf("progress: %d\n",progress);
            }
            fflush(stdout);
            c1 = c2;
            c2 = c;
        }

        fflush (f);
        wait (0);
    }
    // Process the results from the child
    printf("Parent Exiting\n");
    break;
    }

    return 0;
}
