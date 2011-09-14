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

    for(i = 0; i < 100; i ++ ) {
        printf("i: %d\n", i);
        sleep(1);
    }
    return 0;
}
