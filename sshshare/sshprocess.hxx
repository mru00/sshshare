#ifndef SSHPROCESS_H
#define SSHPROCESS_H

#include <stdio.h>

#include <string>

#include "process.hxx"

class SshProcess : Process
{
    const std::string url;

public:
    SshProcess(const std::string& url);
    virtual ~SshProcess();

    void write(const std::string& line);
    void run();
    void join();

    virtual void onStateChange()
    {
        printf("onstatechange\n");
    }
    virtual void onSuccess()
    {
        printf("success!\n");
    }
    virtual void onFail(int code)
    {
        printf("fail [%d]!\n", code);
    }
protected:
private:
};

#endif // SSHPROCESS_H
