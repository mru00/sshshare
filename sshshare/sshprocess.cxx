#include <string>
#include <vector>

#include "sshprocess.hxx"
#include "config.hxx"


using namespace std;

SshProcess::SshProcess(const std::string& url)
:Process(true, false)
, url(url)
{
    //ctor
}

SshProcess::~SshProcess()
{
    //dtor
}

void SshProcess::join()
{
    Process::join();
}

void SshProcess::write(const string& line)
{
    fprintf(p_in, "%s\n", line.c_str());
    fflush(p_out);
    fflush(p_err);
    //usleep(10000);
    sleep(1);
}

void SshProcess::run()
{
    vector<string> argv;
    argv.push_back("/usr/bin/ssh");
    argv.push_back(Config::makeUrl());

    Process::start(argv[0], argv);
}
