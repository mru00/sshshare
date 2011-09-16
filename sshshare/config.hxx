#ifndef CONFIG_H
#define CONFIG_H


#include <memory>
#include "sshshare_config.hxx"

class Config {

private:
    class XmlConfig {
        std::auto_ptr<config_t> xml_config;
        public:
            XmlConfig();
            ~XmlConfig();
            std::string getUsername() { return xml_config->username(); }
            std::string getServer() { return xml_config->server(); }
        private:
            void writeConfig();
    };

public:

    static std::string makeUrl(){
        return config.getUsername() + "@" + config.getServer();
    }
    static std::string makePath(const std::string& path) {
        return makeUrl() + ":" + path;
    }
    static std::string makeHttpUrl(const std::string& sharename) {
        return "http://" + config.getServer() + "/~" + config.getUsername() + "/shares/" + sharename;
    }
    static std::string makeSftpUrl(const std::string& sharename) {
        return "sftp://" + makeUrl() + "/home/"+config.getUsername()+"/public_html/shares/" + sharename;
    }

    static std::string getConfigNamespace() {
        return "http://sisyphus.teil.cc/~mru/sshshare_config.xsd";
    }

    static std::string getDataNamespace() {
        return "http://sisyphus.teil.cc/~mru/sshshare.xsd";
    }

    static XmlConfig config;
    static const std::string xmlfilename;
};

#endif // CONFIG_H
