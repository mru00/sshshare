#include <gtk/gtk.h>
#include <iostream>

#include "sshshare.hxx"
#include "scp.hxx"
#include "shares.hxx"
#include "sshprocess.hxx"
#include "echoprocess.hxx"
#include "config.hxx"

using namespace std;

int main(int , char** )
{
    if(0){
        cout << endl << endl << endl << "sleep" << endl;
        vector<string> argv;
        argv.push_back("/bin/sleep");
        argv.push_back("3");
        EchoProcess echo;
        echo.run(argv[0], argv);
    }


    try {

        vector<string> v;
        v.push_back("-C");
        v.push_back("uname -a");

        cout << endl << endl << endl << "ssh" << endl;
        SshProcess ssh(Config::makeUrl());
        ssh.run(v);
        sleep(1);
        ssh.join();
    }
    catch (ProcessException& e) {
        cerr << "ex: " << e.what() << endl;
    }

    try {
        cout << endl << endl << endl << "ssh" << endl;
        SshProcess ssh(Config::makeUrl());
        ssh.run();
        ssh << "set -xe";
        ssh << "date > ~/iwashere";
        ssh << "echo hi there 8";
        ssh << "echo hi there 7";
        for (int i = 0; i< 100; i++)ssh.write("date > ~/iwashere");
        ssh << "sleep 10";
        ssh.join();
    }
    catch (ProcessException& e) {
        cerr << "ex: " << e.what() << endl;
    }

    {
        cout << endl << endl << endl << "echo" << endl;
        vector<string> argv;
        argv.push_back("/home/mru/dev/06multimedia/sshshare/test_piper/bin/Debug/test_piper");
        EchoProcess echo;
        echo.run(argv[0], argv);
    }
    {
        cout << endl << endl << endl << "scp test.txt" << endl;
        ScpProcess scp(Config::makePath("test.txt"), ".");
        scp.run();
    }
    {
        cout << endl << endl << endl << "scp gpx" << endl;
        ScpProcess scp("/home/mru/test2.gpx", Config::makePath(""));
        scp.run();
    }

}
