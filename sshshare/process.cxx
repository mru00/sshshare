#include <cstring>
#include <iostream>
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
#include <exception>


#include "process.hxx"

using namespace std;


Process::Process(bool ro, bool re, bool pty)
    : redirect_stdout(ro)
    , redirect_stderr(re)
    , use_pty(pty)
    , running(false)
{
}

Process::~Process()
{
}

void Process::start_with_pty(const char* program, char* argv[])
{
    int pfd_in[2];
    int pfd_err[2];
    int fd_out;
    int parent_stderr = -1;

    onStateChange();

    if (pipe(pfd_in))
    {
        perror("pipe in failed!");
        onFail(errno);
        throw ProcessException(StatusCode(errno), strerror(errno));
    }
    if (redirect_stderr)
    {
        if (pipe(pfd_err))
        {
            perror("pipe err failed!");
            onFail(errno);
            throw ProcessException(StatusCode(errno), strerror(errno));
        }
    }
    else
    {
        parent_stderr = dup(STDERR_FILENO);
        if (parent_stderr < 0)
        {
            perror ("ERROR saving old STDERR");
            throw ProcessException(StatusCode(errno), strerror(errno));
        }
    }

    cout << "using pseudoterminal" << endl;
    pid = forkpty(&fd_out, NULL, NULL, NULL);
    if(pid == -1)
    {
        onStateChange();
        onFail(errno);
        perror("failed to forkpty");
        throw ProcessException(StatusCode(errno), strerror(errno));
    }

    // CHILD?
    if (pid == 0)
    {

        close(pfd_in[1]);
        close(pfd_err[0]);


        if (dup2 (pfd_in[0], STDIN_FILENO) < 0)
        {
            perror ("ERROR redirecting stdin");
            exit(10);
        }

        // either redirect to stderr of parent, or to pipe
        if (dup2 (redirect_stderr?pfd_err[1]:parent_stderr, STDERR_FILENO) < 0)
        {
            perror ("ERROR redirecting STDERR");
            exit(12);
        }

        setvbuf (stdout, NULL, _IONBF, 0);
        setvbuf (stderr, NULL, _IONBF, 0);

        close(pfd_in[0]);
        close(pfd_err[1]);

        if (redirect_stderr) close(parent_stderr);

        if (execv(program, argv))
        {
            perror("failed to execv");
            exit(13);
        }
        exit(14);
    }
    close(parent_stderr);

    close(pfd_in[0]);
    close(pfd_err[1]);


    p_in = fdopen(pfd_in[1], "w");
    p_out = fdopen(fd_out, "r");

    if (redirect_stderr)
    {
        p_err = fdopen(pfd_err[0], "r");
    }
    else
    {
        p_err = NULL;
        close(pfd_err[0]);
    }
}


void Process::start_without_pty(const char* program, char* argv[])
{
    int pfd_in[2];
    int pfd_out[2];
    int pfd_err[2];
    int parent_stderr = -1;

    onStateChange();

    if (pipe(pfd_in))
    {
        perror("pipe in failed!");
        onFail(errno);
        throw ProcessException(StatusCode(errno), strerror(errno));
    }

    if (pipe(pfd_out))
    {
        perror("pipe out failed!");
        onFail(errno);
        throw ProcessException(StatusCode(errno), strerror(errno));
    }

    if (redirect_stderr)
    {
        if (pipe(pfd_err))
        {
            perror("pipe err failed!");
            onFail(errno);
            throw ProcessException(StatusCode(errno), strerror(errno));
        }
    }
    else
    {
        parent_stderr = dup(STDERR_FILENO);
        if (parent_stderr < 0)
        {
            perror ("ERROR saving old STDERR");
            throw ProcessException(StatusCode(errno), strerror(errno));
        }
    }



    cout << "NOT using pseudoterminal" << endl;
    pid = fork();
    if(pid == -1)
    {
        onStateChange();
        onFail(errno);
        perror("failed to fork");
        throw ProcessException(StatusCode(errno), strerror(errno));
    }



    // CHILD?
    if (pid == 0)
    {

        close(pfd_in[1]);
        close(pfd_out[0]);
        close(pfd_err[0]);

        if (dup2 (pfd_in[0], STDIN_FILENO) < 0)
        {
            perror ("ERROR redirecting stdin");
            exit(10);
        }

        if (dup2 (pfd_out[1], STDOUT_FILENO) < 0)
        {
            perror ("ERROR redirecting stdout");
            exit(11);
        }

        // either redirect to stderr of parent, or to pipe
        if (dup2 (redirect_stderr? pfd_err[1] : parent_stderr, STDERR_FILENO) < 0)
        {
            perror ("ERROR redirecting STDERR");
            exit(12);
        }

        setvbuf (stdout, NULL, _IONBF, 0);
        setvbuf (stderr, NULL, _IONBF, 0);

        close(pfd_in[0]);
        close(pfd_out[1]);
        close(pfd_err[1]);

        if (redirect_stderr) close(parent_stderr);

        if (execv(program, argv))
        {
            perror("failed to execv");
            exit(13);
        }
        exit(14);
    }

    close(parent_stderr);

    close(pfd_in[0]);
    close(pfd_out[1]);
    close(pfd_err[1]);

    p_in = fdopen(pfd_in[1], "w");
    p_out = fdopen(pfd_out[0], "r");

    if (redirect_stderr)
    {
        p_err = fdopen(pfd_err[0], "r");
    }
    else
    {
        p_err = NULL;
        close(pfd_err[0]);
    }
}


void Process::start(const string& binary, const vector<string>& argv)
{

    char* c_argv[argv.size()+1];
    int i = 0;
    for (vector<string>::const_iterator it = argv.begin(); it!= argv.end(); it ++ )
    {
        c_argv[i++] = (char*)it->c_str();
    }
    c_argv[i++] = NULL;


    start_without_pty(binary.c_str(), c_argv);

    // PARENT
    onStateChange();


    setvbuf ( p_in , NULL , _IONBF , 0);
    setvbuf ( p_out , NULL , _IONBF , 0);
    if(p_err) setvbuf ( p_err , NULL , _IONBF , 0);

    if (true || print_exit_details)
        cerr << "fd's: "
             << p_in << " "
             << p_out << " "
             << p_err << " "
             << (p_in ? fileno(p_in):-1) << " "
             << (p_out ? fileno(p_out):-1) << " "
             << (p_err ? fileno(p_err):-1) << endl;

    running = true;

}


bool Process::isAlive()
{

    //if (running == false) return false;

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
    if (siginfo.si_pid == pid)
    {
        status_last_wait = siginfo.si_status;
        return false;
    }
    return true;
}


void Process::join()
{

    int status = 0, w = 0;

    p_in && fflush(p_in);
    p_in && fclose(p_in);

    w = waitpid (pid, &status, 0);
    if (w==-1)
    {
        perror("waitpid, join");
    }

    p_out && fflush(p_out);
    p_err && fflush(p_err);
    p_out && fclose(p_out);
    p_err && fclose(p_err);

    pid = 0;

    if (status)
    {
        onFail(status);
        throw ProcessException(status, "Process returned with unsuccessful exit code.");
    }
    else onSuccess();


    fflush(NULL);

    pid = 0;
}


bool Process::fd_is_open(FILE* fd)
{
    return fcntl(fileno(fd), F_GETFL) != -1;
}

bool Process::can_read_from_fd(FILE* fd)
{

    if (fd == NULL) return false;
    //if (!isAlive()) return false;

    if (feof(fd))
    {
        return false;
    }
    if (ferror(fd))
    {
//        perror("read from fd");
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
