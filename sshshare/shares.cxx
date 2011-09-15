#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include "shares.hxx"
#include "scp.hxx"
#include "sshshare.hxx"
#include "sshprocess.hxx"
#include "config.hxx"

using namespace std;

void create_share(string name, const users_t& users) {

    if (users.user().size() == 0) {
        printf("no users specified!");
        return;
    }

    string htpasswd("/home/mru/shares/htpasswd."+name);
    string htaccess("public_html/shares/"+name+"/.htaccess");

    ofstream hta("htaccess");
    //hta << "Option +Indexes" << endl;
    //hta << "IndexOptions -FancyIndexing" << endl;
    hta << "AuthUserFile " << htpasswd << endl;
    hta << "AuthGroupFile /dev/null" << endl;
    hta << "AuthName \"share " << name << "\"" << endl;
    hta << "AuthType Basic" << endl;
    hta << "<Limit GET POST>" << endl;
    hta << "require valid-user" << endl;
    hta << "</Limit>" << endl;
    hta.close();


    ofstream script("test.sh");
    script << "#! /bin/bash -xe" << endl;
    script << "mkdir -p ~/public_html/shares/" << name << "" << endl;
    script << "mkdir -p ~/shares" << endl;

    int i = 0;
    for (users_t::user_const_iterator it (users.user().begin ());
            it != users.user().end ();
            ++it, ++i)
    {
        script << "htpasswd " << (i ? "" : "-c") << " -b " << htpasswd << " \"" << it->name() << "\" \"" << it->password() << "\"" << endl;
    }

    script.close();

    ScpProcess scp("test.sh", Config::makePath("create_share.sh"));
    scp.run();


    SshProcess ssh(Config::makeUrl());
    ssh.run();
    ssh.write("sh create_share.sh");
    ssh.join();

    ScpProcess scp1("htaccess", Config::makePath(htaccess));
    scp1.run();

}
