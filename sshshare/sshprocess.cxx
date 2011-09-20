#include <string>
#include <vector>

#include <boost/foreach.hpp>

#include <stdio.h>
#include <iostream>
#include <string>


#include "sshprocess.hxx"
#include "config.hxx"


using namespace std;

SshProcess::SshProcess(const std::string& url)
    :Process(Process::STDOUT_KEEP, Process::STDERR_KEEP, false)
    , url(url)
{
    //ctor
}

SshProcess::~SshProcess()
{
}

void SshProcess::dump_buffers() {
    while (can_read_from_fd(p_err) ) cerr << "[sshprocess/stderr] " << (char)fgetc(p_err) << endl;
    while (can_read_from_fd(p_out) ) cout << "[sshprocess/stdout] " << (char)fgetc(p_out) << endl;
}

void SshProcess::join()
{
    dump_buffers();
    Process::join();
}

void SshProcess::write(const string& line)
{
    if (!isAlive()) {
        throw ProcessException(StatusCode(-1), "State error: process not started!");
    }
    if (p_in == NULL) {
        throw ProcessException(StatusCode(-1), "State error: p_in not open!");
    }


    dump_buffers();
    fprintf(p_in, "%s\n", line.c_str());
    fflush(p_in);
    cerr << "debug: sshwrite: " << line << endl;
    usleep(1000);
}


void SshProcess::run(const vector<string>& additional_arguments)
{
    vector<string> argv;
    argv.push_back("/usr/bin/ssh");
    argv.push_back(Config::makeUrl());
    //argv.push_back("-t");
    argv.insert(argv.end(), additional_arguments.begin(), additional_arguments.end());

    BOOST_FOREACH(string s, argv)
        cerr << "argument vector: "<<s<< endl;

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
