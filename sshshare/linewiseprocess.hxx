#ifndef LINEWISEPROCESS_H
#define LINEWISEPROCESS_H

#include "process.hxx"

class LinewiseProcess : public Process
{
public:
    LinewiseProcess();
    virtual ~LinewiseProcess();

    virtual void onStdout(const std::string& line) = 0;
    virtual void onStderr(const std::string& line) = 0;

    void run(const std::string& binary, const std::vector<std::string>& argv);

protected:
private:
    bool append(std::string& string, char c, void(LinewiseProcess::*cb)(const std::string&));
};

#endif // LINEWISEPROCESS_H
