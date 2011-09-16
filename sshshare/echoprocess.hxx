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
        std::cout << "onstatechange" << std::endl;
    }
    virtual void onSuccess()
    {
        std::cout << "success!" << std::endl;
    }
    virtual void onFail(int code)
    {
        std::cout << "fail [" << code << "]" << std::endl;
    }
    virtual void onProgress(int progress)
    {
        std::cout << "progress: "<< progress << std::endl;
    }
protected:
private:
};

#endif // ECHOPROCESS_H
