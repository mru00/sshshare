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


Process::Process(bool ro, bool re)
    : redirect_stdout(ro)
    , redirect_stderr(re)
{
    //ctor
}

Process::~Process()
{
    //dtor
}



int Process::start(const string& binary, const vector<string>& argv)
{

    int pfd[2];
    int pfd_out[2];
    const int usepty = 1;


    onStateChange();

    if (pipe(pfd))
    {
        perror("pipe failed!");
        onFail(errno);
        return errno;
    }

    if (usepty)
    {
        pid = forkpty(&pfd_out[0], NULL, NULL, NULL);
    }
    else
    {
        pid = fork();

        if (pipe(pfd_out))
        {
            perror("pipe pfd_out failed!");
            onFail(errno);
            return errno;
        }
    }

    int parent_stderr = dup(STDERR_FILENO);

    if(pid == -1)
    {
        onStateChange();
        onFail(errno);
        perror("forkpty");
        return errno;
    }

    // CHILD?
    if (pid == 0)
    {
        int fd_in = pfd[0];
        int fd_err = pfd[1];
        int fd_out = pfd_out[1];
        int fd_unused = pfd_out[0];

        if (dup2 (fd_in, STDIN_FILENO) < 0)
        {
            perror ("ERROR redirecting stdin");
            exit(EXIT_FAILURE);
        }

        if (!usepty)
        {
            if (dup2 (fd_out, STDOUT_FILENO) < 0)
            {
                perror ("ERROR redirecting stdout");
                exit(EXIT_FAILURE);
            }
            close(fd_unused);
            close(fd_out);
        }

        // either redirect to stderr of parent, or to pipe
        if (dup2 (redirect_stderr? fd_err : parent_stderr, STDERR_FILENO) < 0)
        {
            perror ("ERROR redirecting STDERR");
            exit(EXIT_FAILURE);
        }

#if 0
        /* Ensure that terminal echo is switched off so that we
           do not get back from the spawned process the same
           messages that we have sent it. //*/
        struct termios orig_termios;
        if (tcgetattr (fd_out, &orig_termios) < 0)
        {
            perror ("ERROR getting current terminal's attributes");
            return -1;
        }

        orig_termios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
        orig_termios.c_oflag &= ~(ONLCR);

        if (tcsetattr (fd_out, TCSANOW, &orig_termios) < 0)
        {
            perror ("ERROR setting current terminal's attributes");
            return -1;
        }
#endif
        setvbuf (stdout, NULL, _IONBF, 0);
        setvbuf (stderr, NULL, _IONBF, 0);

        close(fd_in);
        close(fd_err);

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

    int fd_in = pfd[1];
    int fd_err = pfd[0];
    int fd_out = pfd_out[0];
    int fd_unused = pfd_out[1];

    if (!usepty) close(fd_unused);

    // PARENT
    onStateChange();
    usleep(100);


    p_in = fdopen(fd_in, "w");
    p_out = fdopen(fd_out, "r");
    p_err = redirect_stderr ? fdopen(fd_err, "r") : NULL;

    setvbuf ( p_in , NULL , _IONBF , 0);
    setvbuf ( p_out , NULL , _IONBF , 0);
    p_err && setvbuf ( p_err , NULL , _IONBF , 0);

    if (print_exit_details)
        printf("fd's: %p %p %p %d %d %d\n", p_in, p_out, p_err, fileno(p_in), p_out ? fileno(p_out):-1, p_err ? fileno(p_err):-1);

    return 0;
}


bool Process::isAlive()
{

    if (pid == 0) return false;

    int w = 0;

    siginfo_t siginfo;
    siginfo.si_pid = 0;
    siginfo.si_status = 0;

    w = waitid(P_PID, pid, &siginfo, WNOHANG | WNOWAIT | WEXITED);

    if (w == -1)
    {
        perror("waitpid");
        return false;
    }
    if (siginfo.si_pid != pid)
    {
        return true;
    }
    pid = 0;
    status_last_wait = siginfo.si_status;
    return false;
}


void Process::join()
{

    int status = 0, w = 0;

    p_in && fflush(p_in);
    p_in && fclose(p_in);

    p_out && fflush(p_out);
    p_err && fflush(p_err);
    p_out && fclose(p_out);
    p_err && fclose(p_err);

    if (isAlive())
    {
        w = waitpid (pid, &status, 0);
        if (w==-1)
        {
            perror("waitpid, join");
        }
    }
    else
    {
        status = status_last_wait;
    }

    if (print_exit_details)
    {

        printf("WIFEXITED: %d\n", WIFEXITED(status));
        printf("WEXITSTATUS: %d\n", WEXITSTATUS(status));
        printf("WIFSIGNALED: %d\n", WIFSIGNALED(status));
        printf("WCOREDUMP: %d\n", WCOREDUMP(status));
        printf("WTERMSIG: %d\n", WTERMSIG(status));
        printf("WCOREDUMP: %d\n", WCOREDUMP(status));
        printf("WIFSTOPPED: %d\n", WIFSTOPPED(status));
        printf("WSTOPSIG: %d\n", WSTOPSIG(status));
        printf("WIFCONTINUED: %d\n", WIFCONTINUED(status));
    }

    if (status) onFail(status);
    else onSuccess();

    pid = 0;
}


bool Process::fd_is_open(FILE* fd)
{
    return fcntl(fileno(fd), F_GETFL) != -1;
}

bool Process::can_read_from_fd(FILE* fd)
{

    //if (!isAlive()) return false;

    if (feof(fd))
    {
        return false;
    }
    if (ferror(fd))
    {
        //  perror("read from fd");
        return false;
    }
    if (!fd_is_open(fd))
    {
        //  return false;
    }

    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(fileno(fd), &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    retval = select(fileno(fd)+1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now! */

    if (retval == -1)
    {
        perror("select()");
        return false;
    }
    return retval > 0;
}
