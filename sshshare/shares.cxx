#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "shares.hxx"
#include "scp.hxx"
#include "sshshare.hxx"
#include "sshprocess.hxx"
#include "config.hxx"

using namespace std;


struct concat
{
    SshProcess& ssh;
    int i;
    string htpasswd;
    concat(SshProcess& ssh, const string& htpasswd) : ssh(ssh), i(0), htpasswd(htpasswd) {}
    void operator()(const user_t& user)
    {
        stringstream ss;
        ss << "htpasswd " << (i++ ? "" : "-c") << " -b " << htpasswd << " \"" << user.name() << "\" \"" << user.password() << "\"";
        ssh.write(ss.str());
    }
};

struct sshwrite
{
    SshProcess& ssh;
    sshwrite(SshProcess& ssh) : ssh(ssh) {}
    void operator()(const string& s)
    {
        ssh.write(s);
    }
};

void create_share(string name, const users_t::user_sequence& users)
{
    if (users.size() == 0)
    {
        printf("no users specified!");
        return;
    }

    string htpasswd("/home/mru/shares/htpasswd."+name);
    string htaccess("public_html/shares/"+name+"/.htaccess");
    string script_fn_local = "/tmp/create_share.sh";

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


    try
    {
        cerr <<  "copy script" << endl;
        {
            ScpProcess(script_fn_local, Config::makePath("shares/create_share.sh")).run();
        }
        cerr << "call script" << endl;

        {
            SshProcess ssh(Config::makeUrl());
            ssh.run();
            ssh.write("set -xe");
            ssh.write("mkdir -p ~/public_html/shares/" + name);
            ssh.write("mkdir -p ~/shares");
            for_each(users.begin(), users.end(), concat(ssh, htpasswd));
            //ssh.write("date > ~/iwashere");
            //ssh.write("exit 0");
            ssh.join();
        }
        cerr << "copy htaccess" << endl;

        ScpProcess("htaccess", Config::makePath(htaccess)).run();
    }
    catch (ProcessException& e)
    {
        cerr << "Exception: " << e.what() << endl;
        throw;
    }
}
