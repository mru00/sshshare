#ifndef _SCP_H
#define _SCP_H

#include "linewiseprocess.hxx"


class ScpProcess : public LinewiseProcess
{
private:
    const std::string from;
    const std::string to;
public:

    ScpProcess(const std::string& from1, const std::string& to1)
        : from(from1), to(to1)
        , LinewiseProcess(Process::STDOUT_PIPE, Process::STDERR_KEEP) {};


    int run()
    {
        std::vector<std::string> argv;
        argv.push_back("/usr/bin/scp");
        argv.push_back("-B");
        argv.push_back(from);
        argv.push_back(to);
        LinewiseProcess::run(argv[0], argv);
        return 0;
    }

public:
    virtual void onStdout(const std::string& line)
    {
        std::cout << "stdout[scp]: " << line << std::endl;

        char c1 = ' ', c2 = ' ';
        int progress;
        int last_progress = -1;

        for (std::string::const_iterator it = line.begin(); it != line.end(); it ++ )
        {
            if (*it == '%')
            {
                if (c1 == ' ')
                    progress = c2 - '0';
                else if (c1 == '0' && c2 == '0')
                    progress = 100;
                else
                    progress = (c1-'0')*10+(c2-'0');

                if (progress != last_progress)
                {
                    last_progress = progress;
                    onProgress(progress);
                }
            }

            c1 = c2;
            c2 = *it;
        }

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
};

#endif
