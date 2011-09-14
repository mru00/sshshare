#ifndef PROCESS_H
#define PROCESS_H


class Process
{
    public:
        Process();
        virtual ~Process();

        virtual void onStateChange() = 0;
        virtual void onStdout(std::string line) = 0;
        virtual void onStderr(std::string line) = 0;
        virtual void onSuccess() = 0;
        virtual void onFail() = 0;

        int run(const std::string& binary, const std::vector<std::string>& argv);

    protected:
    private:
};

#endif // PROCESS_H
