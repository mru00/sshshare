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


LinewiseProcess::LinewiseProcess(Process::STDOUT_MODE mo, Process::STDERR_MODE me) : Process(mo, me, true)
{
    //ctor
}

LinewiseProcess::~LinewiseProcess()
{
    //dtor
}


void LinewiseProcess::run(const std::string& binary, const std::vector<std::string>& argv)
{

    start(binary, argv);
    string b_stdout;
    string b_stderr;
    while (isAlive())
    {

        while (can_read_from_fd(p_err) )
            append(b_stderr, fgetc(p_err), &LinewiseProcess::onStderr);

        while (can_read_from_fd(p_out) )
            append(b_stdout, fgetc(p_out), &LinewiseProcess::onStdout);

    }

    while (can_read_from_fd(p_err) )
        append(b_stderr, fgetc(p_err), &LinewiseProcess::onStderr);

    while (can_read_from_fd(p_out) )
        append(b_stdout, fgetc(p_out), &LinewiseProcess::onStdout);



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
