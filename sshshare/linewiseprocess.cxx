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

#include "linewiseprocess.hxx"

using namespace std;


LinewiseProcess::LinewiseProcess()
{
    //ctor
}

LinewiseProcess::~LinewiseProcess()
{
    //dtor
}


void LinewiseProcess::run(const std::string& binary, const std::vector<std::string>& argv)
{

    Process::start(binary, argv);
    char c;
    string b_stdout;
    string b_stderr;
    while (isAlive())
    {
        fd_set rfds;
        struct timeval tv;
        int retval;

        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(p_out, &rfds);
        FD_SET(p_err, &rfds);

        int max = 0;
        if (p_out > max) max = p_out;
        if (p_err > max) max = p_err;

        /* Wait up to five seconds. */
        tv.tv_sec = 0;
        tv.tv_usec = 2000;

        retval = select(max+1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

        if (retval == -1)
            perror("select()");
        else if (retval)
        {
            /* FD_ISSET(0, &rfds) will be true. */

            if (FD_ISSET(p_err, &rfds))
            {
                read(p_err, &c, 1);
                append(b_stderr, c, &LinewiseProcess::onStderr);
            }
            if (FD_ISSET(p_out, &rfds))
            {
                read(p_out, &c, 1);
                append(b_stdout, c, &LinewiseProcess::onStdout);
            }
        }
    }

    while (can_read_from_fd(p_err) ) { if(read(p_err, &c, 1)<=0) break; append(b_stderr, c, &LinewiseProcess::onStderr);};
    while (can_read_from_fd(p_out) ) { if(read(p_out, &c, 1)<=0) break; append(b_stdout, c, &LinewiseProcess::onStdout);};


    join();
}


bool LinewiseProcess::append(std::string& buffer, char c, void(LinewiseProcess::*cb)(const string&))
{
    if (c == '\n')
    {
        (this->*cb)(buffer);
        buffer.clear();
        fflush(stdout);
        return true;
    }

    if (isprint(c))
        buffer.push_back(c);

    return false;
}
