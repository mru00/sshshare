#include <vector>


#include "echoprocess.hxx"

EchoProcess::EchoProcess()
: LinewiseProcess(Process::STDOUT_KEEP, Process::STDERR_KEEP)
{
    //ctor
}

EchoProcess::~EchoProcess()
{
    //dtor
}
