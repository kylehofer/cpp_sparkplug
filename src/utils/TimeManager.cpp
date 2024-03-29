#include "TimeManager.h"
#include "stdint.h"

#if defined(PICO) || defined(PICO_RP2040)
#include "pico/stdlib.h"
#endif

class BasicTimeManager : TimeClient
{
public:
    virtual time_t getTime() override
    {
#if defined(PICO_RP2040) || defined(PICO)
        return us_to_ms(time_us_64());
#else
        // Set the timestamp
        struct timespec ts;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        ts.tv_sec = mts.tv_sec;
        ts.tv_nsec = mts.tv_nsec;
#else
        clock_gettime(CLOCK_REALTIME, &ts);
#endif
        return ts.tv_sec * UINT64_C(1000) + ts.tv_nsec / 1000000;
#endif
    }
};

inline static BasicTimeManager basicManager;

time_t TimeManager::getTime()
{
    return TimeManager::client ? TimeManager::client->getTime() : basicManager.getTime();
}

void TimeManager::setInstance(TimeClient *client)
{
    TimeManager::client = client;
}

void TimeManager::reset()
{
    TimeManager::setInstance((TimeClient *)&basicManager);
}
