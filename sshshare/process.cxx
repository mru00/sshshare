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


Process::Process(STDOUT_MODE stdout_mode, STDERR_MODE stderr_mode, bool pty)
    : p_in(NULL)
    , p_out(NULL)
    , p_err(NULL)
    , stdout_mode(stdout_mode)
    , stderr_mode(stderr_mode)
    , use_pty(pty)
    , running(false)
{
}

Process::~Process()
{
    if (pid != 0) {
        cerr << "Process should have been joined!" << endl;
    }
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
    switch (stderr_mode) {

      case STDERR_PIPE:
        if (pipe(pfd_err))
          {
            perror("pipe err failed!");
            onFail(errno);
            throw ProcessException(StatusCode(errno), strerror(errno));
          }
        break;
      case STDERR_KEEP:
          {
            parent_stderr = dup(STDERR_FILENO);
            if (parent_stderr < 0)
              {
                perror ("ERROR saving old STDERR");
                throw ProcessException(StatusCode(errno), strerror(errno));
              }
          }
        break;
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

        int stderr_target;
        switch (stderr_mode) {
          case STDERR_KEEP:
            stderr_target = parent_stderr;
            break;
          case STDERR_PIPE:
            stderr_target = pfd_err[1];
            close(parent_stderr);
            break;
        }
        // either redirect to stderr of parent, or to pipe
        if (dup2 (stderr_target, STDERR_FILENO) < 0)
        {
            perror ("ERROR redirecting STDERR");
            exit(12);
        }

        setvbuf (stdout, NULL, _IONBF, 0);
        setvbuf (stderr, NULL, _IONBF, 0);

        close(pfd_in[0]);
        close(pfd_err[1]);

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

    switch (stderr_mode) {
      case STDERR_KEEP:
        p_err = NULL;
        close(pfd_err[0]);
        break;
      case STDERR_PIPE:
        p_err = fdopen(pfd_err[0], "r");
        break;
    }
}


void Process::start_without_pty(const char* program, char* argv[])
{
    int pfd_in[2];
    int pfd_out[2];
    int pfd_err[2];
    int parent_stderr = -1, parent_stdout = -1;

    onStateChange();

    if (pipe(pfd_in))
    {
        perror("pipe in failed!");
        onFail(errno);
        throw ProcessException(StatusCode(errno), strerror(errno));
    }

    switch(stdout_mode) {
      case STDOUT_PIPE:

        if (pipe(pfd_out))
          {
            perror("pipe out failed!");
            onFail(errno);
            throw ProcessException(StatusCode(errno), strerror(errno));
          }

        break;
      case STDOUT_KEEP:
        parent_stdout = dup(STDOUT_FILENO);
        break;
    }

    switch (stderr_mode) {

      case STDERR_PIPE:
        if (pipe(pfd_err))
          {
            perror("pipe err failed!");
            onFail(errno);
            throw ProcessException(StatusCode(errno), strerror(errno));
          }
        break;
      case STDERR_KEEP:
            parent_stderr = dup(STDERR_FILENO);
        break;
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
      
        int stdout_target;
        switch (stdout_mode) {
          case STDOUT_KEEP:
            stdout_target = parent_stdout;
            cout << "jeep"<<endl;
            break;
          case STDERR_PIPE:
            cout << "pipe" << endl;
            stdout_target = pfd_out[1];
            break;
        }
        if (dup2 (stdout_target, STDOUT_FILENO) < 0)
        {
            perror ("ERROR redirecting stdout");
            exit(11);
        }

        int stderr_target;
        switch (stderr_mode) {
          case STDERR_KEEP:
            stderr_target = parent_stderr;
            break;
          case STDERR_PIPE:
            stderr_target = pfd_err[1];
            close(parent_stderr);
            break;
        }
        // either redirect to stderr of parent, or to pipe
        if (dup2 (stderr_target, STDOUT_FILENO) < 0)
        {
            perror ("ERROR redirecting stderr");
            exit(11);
        }

        setvbuf (stdout, NULL, _IONBF, 0);
        setvbuf (stderr, NULL, _IONBF, 0);

        close(pfd_in[0]);
        close(pfd_out[1]);
        close(pfd_err[1]);

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
    
    switch(stdout_mode) {
      case STDOUT_KEEP:
        p_out = NULL;
        close(pfd_out[0]);
        break;
      case STDOUT_PIPE:
        p_out = fdopen(pfd_out[0], "r");
        break;
    }


    switch (stderr_mode) {
      case STDERR_KEEP:
        p_err = NULL;
        close(pfd_err[0]);
        break;
      case STDERR_PIPE:
        p_err = fdopen(pfd_err[0], "r");
        break;
    }
}


void Process::start(const string& binary, const vector<string>& argv)
{
    cerr << "staring binary: " << binary << endl;
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
    if(p_out) setvbuf ( p_out , NULL , _IONBF , 0);
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
    p_in = NULL;

    w = waitpid (pid, &status, 0);
    if (w==-1)
    {
        perror("waitpid, join");
    }

    p_out && fflush(p_out);
    p_err && fflush(p_err);
    p_out && fclose(p_out);
    p_err && fclose(p_err);


    p_out = p_err = NULL;

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
    if (ferror(fd))
    {
//        perror("read from fd");
        return false;
    }
    if (feof(fd))
    {
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
