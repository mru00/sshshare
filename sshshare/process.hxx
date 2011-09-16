#ifndef PROCESS_H
#define PROCESS_H

#include <sstream>

#include <sys/wait.h>





class StatusCode
{
public:
    int status;
    StatusCode(int c) : status(c) {}

    bool isExited()
    {
        return WIFEXITED(status);
    }
    int exitStatus()
    {
        return WEXITSTATUS(status);
    }
    bool isSignaled()
    {
        return WIFSIGNALED(status);
    }
    bool hasCoreDump()
    {
        return WCOREDUMP(status);
    }
    bool termSig()
    {
        return WTERMSIG(status);
    }
    bool isStopped()
    {
        return WIFSTOPPED(status);
    }
    int stopSig()
    {
        return WSTOPSIG(status);
    }
    bool isContinued()
    {
        return WIFCONTINUED(status);
    }

    std::string describe() {
        std::stringstream ss;
        ss << "code: " << status << std::endl;
        ss << "isExited: " << isExited() << std::endl;
        ss << "exitStatus: " << exitStatus() << std::endl;
        ss << "isSignaled: " << isSignaled() << std::endl;
        ss << "signal: " << termSig() << std::endl;
        ss << "hasCoreDump: " << hasCoreDump() << std::endl;
        return ss.str();
    }
};

class ProcessException : public std::exception
{
public:
    StatusCode status;
    std::string description;


    ProcessException(StatusCode s, const std::string& description)
        : status(s)
        , description(description)
    {}

    virtual ~ProcessException() throw() {};
    virtual const char* what() { return (description + "\n" + status.describe()).c_str(); }
};


class Process
{
    const static bool print_exit_details = false;

public:
    Process(bool ro = true, bool re = false, bool pty=false);
    virtual ~Process();

    virtual void onStateChange() = 0;
    virtual void onSuccess() = 0;
    virtual void onFail(int code) = 0;

    virtual void start(const std::string& binary, const std::vector<std::string>& argv);

    FILE* p_in;
    FILE* p_out;
    FILE* p_err;

    virtual void join();
    virtual bool isAlive();


    virtual bool fd_is_open(FILE* fd);
    virtual bool can_read_from_fd(FILE* fd);

private:

    void start_with_pty(const char* program, char* argv[]);
    void start_without_pty(const char* program, char* argv[]);

protected:
private:
    int pid;

    int status_last_wait;
    bool redirect_stdout;
    bool redirect_stderr;
    bool use_pty;
    bool running;
};



#endif // PROCESS_H
