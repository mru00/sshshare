#ifndef PROCESS_H
#define PROCESS_H

#include <sys/wait.h>

#include <string>
#include <vector>


class StatusCode
{
public:
    int status;

    bool isExited() { return WIFEXITED(status); }
    int exitStatus() { return WEXITSTATUS(status); }
    bool isSignaled() { return WIFSIGNALED(status); }
    bool hasCoreDump() { return WCOREDUMP(status); }
    bool isTermSig() { return WTERMSIG(status); }
    bool isStopped() { return WIFSTOPPED(status); }
    int stopSig() {  return WSTOPSIG(status); }
    bool isContinued() { return WIFCONTINUED(status); }
};


class Process
{
    const static bool print_exit_details = false;

public:
    Process(bool ro = true, bool re = false);
    virtual ~Process();

    virtual void onStateChange() = 0;
    virtual void onSuccess() = 0;
    virtual void onFail(int code) = 0;

    virtual int start(const std::string& binary, const std::vector<std::string>& argv);

    FILE* p_in;
    FILE* p_out;
    FILE* p_err;

    virtual void join();
    bool isAlive();


    bool fd_is_open(FILE* fd);
    bool can_read_from_fd(FILE* fd);

protected:
private:
    int pid;
    int status_last_wait;
    bool redirect_stdout;
    bool redirect_stderr;
};



#endif // PROCESS_H
