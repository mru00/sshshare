#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>


#include "shares.hxx"
#include "scp.hxx"
#include "sshshare.hxx"
#include "sshprocess.hxx"
#include "config.hxx"

using namespace std;


void create_share(string name, const users_t::user_sequence& users)
{
    if (users.size() == 0)
    {
        printf("no users specified!");
        return;
    }

    string htpasswd("/home/" + Config::getUsername() + "/shares/htpasswd."+name);
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
            int i  = 0;
            SshProcess ssh(Config::makeUrl());
            ssh.run();
            ssh.write("set -xe");
            ssh.write("mkdir -p ~/public_html/shares/" + name);
            ssh.write("mkdir -p ~/shares");
            BOOST_FOREACH(const user_t& user, users)
            {
                stringstream ss;
                ss << "htpasswd " << (i++ ? "" : "-c") << " -b " << htpasswd << " \"" << user.name() << "\" \"" << user.password() << "\"";
                ssh.write(ss.str());
            }
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

void delete_share(const share_t& share, bool keep_data) {

    try {
        SshProcess ssh(Config::makeUrl());
        ssh.run();
        ssh.write("rm ~/shares/htpasswd." + share.name());

        if (keep_data) {

            string sharedir = "~/public_html/shares/" + share.name();
            string deldir = "~/shares/deleted_shares/";

            ssh.write("if [ -d " + sharedir + " ]; then mkdir -p " + deldir + " ; mv "+sharedir + " " + deldir + "; fi");
        }
        else {
            ssh.write("rm -rf ~/public_html/shares/" + share.name());
        }

        ssh.join();
    }
    catch (ProcessException& e) {
        cerr << "Exception: " << e.what() << endl;
        throw;
    }

}
