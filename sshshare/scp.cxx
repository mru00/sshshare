#include <pty.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <string>
#include <vector>

#include "scp.hxx"
#include "process.hxx"

using namespace std;

// http://cwshep.blogspot.com/2009/06/showing-scp-progress-using-zenity.html
// http://rmathew.blogspot.com/2006/09/terminal-sickness.html

class ScpProcess : public Process
{

public:
    virtual void onStdout(string line)
    {
        for (string::iterator it = line.begin(); it != line.end(); it ++ )
        {
            char c1, c2;
            int progress;
            int last_progress = -1;

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
            fflush(stdout);
            c1 = c2;
            c2 = *it;
        }

    }
    virtual void onStderr(string line)
    {
    }
    virtual void onStateChange()
    {
        printf("onstatechange");
    }
    virtual void onSuccess()
    {
        printf("success!");
    }
    virtual void onFail()
    {
        printf("fail!");
    }

    virtual void onProgress(int progress)
    {
        printf("progress: %d\n", progress);
    }
};



int get_file(const char* filename, void (*on_progress)(int), void (*on_status_change)(int, const char*))
{
    char fn_from[FILENAME_MAX];
    char fn_to[FILENAME_MAX];


    snprintf(fn_from, FILENAME_MAX, "%s", filename);
    snprintf(fn_to, FILENAME_MAX, "mru@sisyphus.teil.cc:%s", "test.txt");


    vector<string> argv;
    argv.push_back("/usr/bin/scp");
    argv.push_back("-B");
    argv.push_back(fn_to);
    argv.push_back(".");

    ScpProcess scp;

    return scp.run(argv[0], argv);

}
