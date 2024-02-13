/*
 * File: TestMqttClient.h
 * Project: cpp_sparkplug
 * Created Date: Sunday January 28th 2024
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2024 Kyle Hofer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * HISTORY:
 */

#ifndef TESTS_UTILS_TESTMQTTCLIENT
#define TESTS_UTILS_TESTMQTTCLIENT

#include "clients/CppMqttClient.h"
#include "TestTcpClient.h"

class TestMqttClient : public CppMqttClient
{
private:
    TestTcpClient tcpClient;

protected:
    virtual Client *getClient() override
    {
        return (Client *)&tcpClient;
    };

public:
    /**
     * @brief Construct a new Pico MQTT Client
     *
     * @param handler The Event Handler that manages the callbacks from the Client
     * @param options The options for configuring the MQTT Client
     */
    TestMqttClient(ClientEventHandler *handler, ClientOptions *options) : CppMqttClient(handler, options)
    {
    }

    virtual time_t getTime() override
    {
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
    }
};

#endif /* TESTS_UTILS_TESTMQTTCLIENT */
