#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <boost/filesystem.hpp>
#include <fstream>

#include "config.hxx"

using namespace std;

namespace fs = boost::filesystem;


static string getConfigPath()
{
    struct passwd *pw = getpwuid(getuid());
    fs::path full_path = fs::path(pw->pw_dir) / fs::path(".sshshare_config.xml");
    return full_path.string();
}

Config::XmlConfig::XmlConfig()
{
    string config_path = getConfigPath();

    cout << "configuration filename: " << config_path << endl;

    if (fs::exists(fs::status(config_path)))
    {
        try
        {
            cerr << config_path << endl;
            xml_config = configuration(config_path);
            cerr << "configuration file loaded"<< endl;
        }
        catch (xml_schema::parsing& e)
        {
            cerr << "Exception: " << e << endl;
            cerr << "this is a terminal failure. quitting." << endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        cerr << "configuration file not found! creating default config." << endl;
        xml_config.reset(new config_t("user","www.example.com"));

        writeConfig();
    }
}

void Config::XmlConfig::writeConfig()
{
    cerr << "Writing new config." << endl;

    string config_path = getConfigPath();

    xml_schema::namespace_infomap map;
    map[""].name = "";
    map[""].schema = getConfigNamespace();

    std::ofstream ofs (config_path.c_str());
    configuration (ofs, *xml_config, map);
}

Config::XmlConfig::~XmlConfig()
{
    writeConfig();
}

Config::XmlConfig Config::config;

const std::string Config::xmlfilename = "sharedata.xml";

