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


int Process::run(const string& binary, const vector<string>& argv)
{

    int pid;
    int status;
    int read_fd;
    int pfd[2];

    int saved_stderr = dup (STDERR_FILENO);


    onStateChange();

    if (pipe(pfd)) {
        onFail();
        return errno;
    }

    pid = forkpty(&read_fd, NULL, NULL, NULL);

    if(pid == -1)
    {
        onStateChange();
        onFail();
        return errno;
    }


    // CHILD?
    if (pid == 0)
    {
        /* Restore stderr. */
        if (dup2 (pfd[1], STDERR_FILENO) < 0)
        {
            perror ("ERROR restoring STDERR");
            return -1;
        }

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

    FILE* f = fdopen(read_fd, "r");
    char c;

    string b_stdout;


    while((c = fgetc(f)) != -1)
    {
        if (c == '\n')
        {
            onStdout(b_stdout);
            b_stdout.clear();
        }
        else
        {
            b_stdout.push_back(c);
        }

        fflush(stdout);
    }

    fflush (f);
    waitpid (pid, &status, 0);

    if (status) onFail();
    else onSuccess();

    fflush (stdout);

    return 0;

}
