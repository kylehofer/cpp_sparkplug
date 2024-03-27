#ifndef SRC_UTILS_TIMEMANAGER
#define SRC_UTILS_TIMEMANAGER

#include <time.h>
#include <functional>

typedef std::function<time_t()> TimeFunc;

class BasicTimeManager;

class TimeClient
{
public:
    virtual time_t getTime() = 0;
};

class TimeManager
{
private:
    inline static TimeClient *client = nullptr;

public:
    static time_t getTime();
    static void setInstance(TimeClient *client);
    static void reset();
};

#endif /* SRC_UTILS_TIMEMANAGER */
