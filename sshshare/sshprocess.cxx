#include <string>
#include <vector>

#include "sshprocess.hxx"

using namespace std;

SshProcess::SshProcess()
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

    FILE* f_in = fdopen(p_in, "w");
/*
    if (!fd_is_open(p_in)) {
        printf("p_in is not open!!!\n");
    }

    if (!wait_for_write(p_in)) {
        printf("p_in not ready!!!");
    }
*/
    fprintf(f_in, "%s\n", line.c_str());

    fflush(f_in);
    fflush(fdopen(p_err,"r"));
    fflush(fdopen(p_out,"r"));

#if 0

    char c;
    while (can_read_from_fd(p_err) )
    {
        read(p_err, &c, 1);
        printf("stderr: %c %d\n", c,c);
    }
    while (can_read_from_fd(p_out) )
    {
        read(p_out, &c, 1);
        printf("stdout: %c %d\n", c,c);
    }

#endif
fflush(NULL);
}

void SshProcess::run()
{

#if 0
    vector<string> argv;
    argv.push_back("/usr/bin/ssh");
    argv.push_back("mru@sisyphus.teil.cc");
#else
    vector<string> argv;
    //argv.push_back("/home/mru/dev/06multimedia/sshshare/test_piper/bin/Debug/test_piper");
    argv.push_back("/usr/bin/tee");
    argv.push_back("/home/mru/teetest");

#endif
    Process::start(argv[0], argv);

}
