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

    static const std::string servername;
    static const std::string username;
};

#endif // CONFIG_H
