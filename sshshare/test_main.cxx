#include <gtk/gtk.h>
#include <iostream>

#include "sshshare.hxx"
#include "scp.hxx"
#include "shares.hxx"
#include "sshprocess.hxx"
#include "echoprocess.hxx"



int main(int argc, char** argv)
{
    {
        cout << endl << endl << endl << "ssh" << endl;
        SshProcess ssh;
        ssh.run();
        ssh.write("hi there!");
        ssh.write("hi there!");
        ssh.write("hi there!");
        ssh.join();
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
        ScpProcess scp("mru@sisyphus.teil.cc:test.txt", ".");
        scp.run();
    }
    {
        cout << endl << endl << endl << "scp gpx" << endl;
        ScpProcess scp("/home/mru/test2.gpx", "mru@sisyphus.teil.cc:");
        scp.run();
    }

}
