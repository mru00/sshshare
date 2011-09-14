#ifndef ECHOPROCESS_H
#define ECHOPROCESS_H

#include <stdio.h>
#include <iostream>

#include "linewiseprocess.hxx"

using namespace std;

class EchoProcess : public LinewiseProcess
{
public:
    EchoProcess();
    virtual ~EchoProcess();

    virtual void onStdout(const std::string& line)
    {
        std::cout << "stdout: " << line << std::endl;
    }


    virtual void onStderr(const std::string& line)
    {
        std::cout << "stderr: " << line << std::endl;
    }
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

    virtual void onProgress(int progress)
    {
        printf("progress: %d\n", progress);
    }
protected:
private:
};

#endif // ECHOPROCESS_H
