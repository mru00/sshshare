#ifndef SSHPROCESS_H
#define SSHPROCESS_H




#include "process.hxx"

class SshProcess : Process
{
    const std::string url;

public:
    SshProcess(const std::string& url);
    virtual ~SshProcess();

    virtual void run(const std::vector<std::string>& additional_arguments = std::vector<std::string>());

    virtual void write(const std::string& line);
    virtual void join();

    virtual void onStateChange()
    {
        std::cout << "onstatechange" << std::endl;
    }
    virtual void onSuccess()
    {
        std::cout << "success" << std::endl;
    }
    virtual void onFail(int code)
    {
        std::cout << "fail [" << code << "]" << std::endl;
    }

    void dump_buffers();

    //SshProcess& operator << (const char*);
protected:
private:
};


SshProcess& operator << (SshProcess&, const char*);
SshProcess& operator << (SshProcess&, const std::string&);


#endif // SSHPROCESS_H
