#ifndef TESTS_UTILS_MOCKTIMEMANAGER
#define TESTS_UTILS_MOCKTIMEMANAGER

#include "utils/TimeManager.h"
#include "stdint.h"

class MockTimeManager : TimeClient
{
private:
    time_t time = 0;

public:
    virtual time_t getTime() override
    {
        return time;
    }

    void setTime(time_t time)
    {
        this->time = time;
    }
};

#endif /* TESTS_UTILS_MOCKTIMEMANAGER */
