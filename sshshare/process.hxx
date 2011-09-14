#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <vector>

class Process
{

    public:
        Process();
        virtual ~Process();

        virtual void onStateChange() = 0;
        virtual void onSuccess() = 0;
        virtual void onFail(int code) = 0;

        virtual int start(const std::string& binary, const std::vector<std::string>& argv);

        int p_in;
        int p_out;
        int p_err;

        virtual void join();
        bool isAlive();


        static bool fd_is_open(int fd);
        static bool can_read_from_fd(int fd);

        static bool wait_for_write(int fd);
    protected:
    private:
        int pid;


};



#endif // PROCESS_H
