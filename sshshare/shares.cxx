#include <string>
#include <fstream>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>


#include "shares.hxx"
#include "scp.hxx"
#include "sshshare.hxx"
#include "sshprocess.hxx"
#include "config.hxx"

using namespace std;
using boost::format;
using boost::io::group;


void create_share(string name, const users_t::user_sequence& users)
{
    if (users.size() == 0)
    {
        cerr << "no users specified! not creating htpasswd" << endl;

        try
        {


            string fn_htpasswd = "~/shares/htpasswd." + name;
            string fn_htaccess = "~/public_html/shares/" + name + ".htaccess";

            SshProcess ssh(Config::makeUrl());
            ssh.run();
            ssh.write("set -xe");
            ssh.write("mkdir -p ~/public_html/shares/" + name);
            ssh.write("mkdir -p ~/shares");
            ssh.write(str(format("if [ -f %1% ]; then rm %1%; fi") % fn_htpasswd));
            ssh.write(str(format("if [ -f %1% ]; then rm %1%; fi") % fn_htaccess));
            ssh.write("true");

            ssh.join();
        }
        catch (ProcessException& e)
        {
            cerr << "Exception: " << e.what() << endl;
            throw;
        }

    }
    else
    {

        string htpasswd("/home/" + Config::getUsername() + "/shares/htpasswd."+name);
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


        try
        {

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
                    ssh.write(str(format("htpasswd %1% -b %2% \"%3%\" \"%4%\"") % (i++ ? "" : "-c") % htpasswd % user.name() % user.password() ));
                }

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
}

void delete_share(const share_t& share, bool keep_data)
{

    try
    {
        string fn_htpasswd = "~/shares/htpasswd." + share.name();
        string sharedir = "~/public_html/shares/" + share.name();
        string deldir = "~/shares/deleted_shares/";


        SshProcess ssh(Config::makeUrl());
        ssh.run();

        ssh.write(str(format("if [ -f %1% ]; then rm %1%; fi") % fn_htpasswd));

        if (keep_data)
        {
            ssh.write(str(format("if [ -d %1% ]; then mkdir -p %2%; mv %1% %2%; fi") % sharedir % deldir));
        }
        else
        {
            ssh.write("rm -rf " + sharedir);
        }

        ssh.join();
    }
    catch (ProcessException& e)
    {
        cerr << "Exception: " << e.what() << endl;
        throw;
    }

}
