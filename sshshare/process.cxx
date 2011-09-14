#include <pty.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>


#include <string>
#include <vector>



#include "process.hxx"

using namespace std;


Process::Process()
{
    //ctor
}

Process::~Process()
{
    //dtor
}



int Process::start(const string& binary, const vector<string>& argv)
{

    int fd_stdout;
    int pfd[2];


    onStateChange();

    if (pipe(pfd))
    {
        perror("pipe failed!");
        onFail(errno);
        return errno;
    }

    pid = forkpty(&fd_stdout, NULL, NULL, NULL);

    if(pid == -1)
    {
        onStateChange();
        onFail(errno);
        return errno;
    }


    // CHILD?
    if (pid == 0)
    {

        if (dup2 (pfd[0], STDIN_FILENO) < 0)
        {
            perror ("ERROR restoring STDERR");
            return -1;
        }


        if (dup2 (pfd[1], STDERR_FILENO) < 0)
        {
            perror ("ERROR restoring STDERR");
            return -1;
        }
        close(pfd[1]);
        close(pfd[0]);

        char* c_argv[argv.size()+1];
        int i = 0;
        for (vector<string>::const_iterator it = argv.begin(); it!= argv.end(); it ++ )
        {
            c_argv[i++] = (char*)it->c_str();
        }
        c_argv[i++] = NULL;

        if (execv(binary.c_str(), c_argv))
        {
            perror("failed to execv");
            exit(EXIT_FAILURE);
        }
        return -1;
    }

    // PARENT
    onStateChange();

    p_in = pfd[1];
    p_out = fd_stdout;
    p_err = pfd[0];


    printf("fd's: %d %d %d\n", p_in, p_out, p_err);

    return 0;
}

bool Process::isAlive()
{
    int status = 0;
    return waitpid (pid, &status, WNOHANG) == 0;
}


void Process::join()
{

    close(p_in);
    close(p_out);
    close(p_err);

    int status = 0;
    waitpid (pid, &status, 0);

    printf("WIFEXITED: %d\n", WIFEXITED(status));
    printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
    printf("WIFSIGNALED: %d\n", WIFSIGNALED(status));
    printf("WCOREDUMP: %d\n", WCOREDUMP(status));
    printf("WTERMSIG: %d\n", WTERMSIG(status));
    printf("WCOREDUMP: %d\n", WCOREDUMP(status));
    printf("WIFSTOPPED: %d\n", WIFSTOPPED(status));
    printf("WSTOPSIG: %d\n", WSTOPSIG(status));
    printf("WIFCONTINUED: %d\n", WIFCONTINUED(status));


    if (status) onFail(status);
    else onSuccess();

    fflush (NULL);
}


bool Process::fd_is_open(int fd) {
    return fcntl(fd, F_GETFL) != -1;
}

bool Process::wait_for_write(int fd)
{


    fd_set wfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);

    int max = 0;
    if (fd > max) max = fd;

    /* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 2000;

    retval = select(max+1, NULL, &wfds, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
    {
        perror("select()");
        return false;
    }
    return retval > 0;
}

bool Process::can_read_from_fd(int fd)
{

    if (!fd_is_open(fd)) return false;

    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    int max = 0;
    if (fd > max) max = fd;

    /* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 200;

    retval = select(max+1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
    {
        perror("select()");
        return false;
    }
    return retval > 0;
}
