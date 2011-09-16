#include <string>
#include <vector>

#include <stdio.h>
#include <iostream>
#include <string>


#include "sshprocess.hxx"
#include "config.hxx"


using namespace std;

SshProcess::SshProcess(const std::string& url)
    :Process(true, true, false)
    , url(url)
{
    //ctor
}

SshProcess::~SshProcess()
{
    //dtor
}

void SshProcess::dump_buffers() {
    while (p_err && can_read_from_fd(p_err) )
    {
        cerr << "[sshprocess/stderr] " << (char)fgetc(p_err) << endl;
    };
    while (p_out && can_read_from_fd(p_out) )
    {
        cout << "[sshprocess/stdout] " << (char)fgetc(p_out) << endl;
    };

}

void SshProcess::join()
{
    //write("exit");
    dump_buffers();
    Process::join();
}

void SshProcess::write(const string& line)
{
    dump_buffers();
    fprintf(p_in, "%s\n", line.c_str());
    fflush(p_in);
    cerr << "debug: sshwrite: " << line << endl;
    usleep(1000);
}

void print_vector(const string& str)
{
    cerr << "argument vector: " << str << endl;
}

void SshProcess::run(const vector<string>& additional_arguments)
{
    vector<string> argv;
    argv.push_back("/usr/bin/ssh");
    argv.push_back(Config::makeUrl());
    //argv.push_back("-t");
    argv.insert(argv.end(), additional_arguments.begin(), additional_arguments.end());

    for_each(argv.begin(), argv.end(), print_vector);

    Process::start(argv[0], argv);
}

SshProcess& operator << (SshProcess& p, const char* v) {
    p.write(v);
    return p;
}

SshProcess& operator << (SshProcess& p, const std::string& v) {
    p.write(v);
    return p;
}
