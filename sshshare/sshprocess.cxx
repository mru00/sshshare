#include <string>
#include <vector>

#include "sshprocess.hxx"

using namespace std;

SshProcess::SshProcess()
:Process(true, false)
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
    usleep(10000);
}

void SshProcess::run()
{

#if 1
    vector<string> argv;
    argv.push_back("/usr/bin/ssh");
    argv.push_back("mru@sisyphus.teil.cc");
    argv.push_back("-C");
    argv.push_back("echo sdf2435 > test.txt");
#else
    vector<string> argv;
    //argv.push_back("/home/mru/dev/06multimedia/sshshare/test_piper/bin/Debug/test_piper");
    argv.push_back("/usr/bin/tee");
    argv.push_back("/home/mru/teetest");

#endif
    Process::start(argv[0], argv);

}
