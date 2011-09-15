#ifndef CONFIG_H
#define CONFIG_H


class Config {

public:

    static std::string makeUrl(){
        return username + "@" + servername;
    }
    static std::string makePath(const std::string& path) {
        return makeUrl() + ":" + path;
    }
    static std::string makeHttpUrl(const std::string& sharename) {
        return "http://" + servername + "/~" + username + "/shares/" + sharename;
    }
    static std::string makeSftpUrl(const std::string& sharename) {
        return "sftp://" + makeUrl() + "/home/"+username+"/public_html/shares/" + sharename;
    }


    static const std::string servername;
    static const std::string username;
    static const std::string xmlfilename;
    static const std::string xsdfilename;
};

#endif // CONFIG_H
