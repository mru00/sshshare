#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include "shares.hxx"
#include "scp.hxx"
#include "sshshare.hxx"
#include "sshprocess.hxx"


using namespace std;

void create_share(string name, const users_t& users) {

    ofstream script("test.sh");
    script << "#! /bin/bash -xe" << endl;
    script << "mkdir -p ~/public_html/shares/" + name << endl;
    script << "cd ~/public_html/shares/"+name << endl;

    int i = 0;
    for (users_t::user_const_iterator it (users.user().begin ());
            it != users.user().end ();
            ++it, ++i)
    {
        script << "htpasswd " << (i ? "" : "-c") << " -b \"" << it->name() << "\" \"" << it->password() << "\"" << endl;
    }
    script.close();


    ScpProcess scp("test.sh", "mru@sisyphus.teili.cc:create_share.sh");
    scp.run();

    SshProcess ssh;
    ssh.run();
    for (int j = 0; j < 10; j ++)
        ssh.write("echo hi!");

    ssh.join();

}
