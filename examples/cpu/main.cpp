/*
 * File: BrokerTests.cpp
 * Project: cpp_sparkplug
 * Created Date: Sunday November 27th 2022
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2022 Kyle Hofer
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

#ifdef EXAMPLE_ASYNC
#include "clients/PahoAsyncClient.h"
#else
#include "clients/PahoSyncClient.h"
#endif

#include "Node.h"
#include "Device.h"

#include <thread>
#include <chrono>
#include <vector>
#include <ctime>
#include "CPU.h"

#define MAX_RETRIES 400
#define EXECUTE_TIME 500

using namespace std;
using ::chrono::milliseconds;
using ::this_thread::sleep_for;

int main(int argc, char *argv[])
{
    char groupName[256];
    char nodeName[256];
    char clientAddress[256];
    char clientId[256];

    if (argc > 1)
    {
        sprintf(groupName, "%s", argv[1]);
    }
    else
    {
        sprintf(groupName, "Examples");
    }

    if (argc > 2)
    {
        sprintf(nodeName, "%s", argv[2]);
    }
    else
    {
        sprintf(nodeName, "Computer");
    }

    if (argc > 3)
    {
        sprintf(clientAddress, "%s", argv[3]);
    }
    else
    {
        sprintf(clientAddress, "tcp://localhost:1883");
    }

    if (argc > 4)
    {
        sprintf(clientId, "%s", argv[4]);
    }
    else
    {
        std::srand(std::time(nullptr));
        int random = std::rand();
        sprintf(clientId, "sparkplug_client_%d", random);
    }

    NodeOptions nodeOptions = {
        groupName,
        nodeName,
        std::string(),
        1000,
        NODE_CONTROL_REBIRTH};

    Uri uri(clientAddress);

    ClientOptions clientOptions = {
        .address = uri,
        .clientId = clientId,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 1000,
        .keepAliveInterval = 15};

    Node node = Node(&nodeOptions);

    auto cpus = CPU::getCpus();

    for (auto &cpu : cpus)
    {
        node.addDevice(cpu);
    }

    CPU::updateValues();

#ifdef EXAMPLE_ASYNC
    node.addClient<PahoAsyncClient>(&clientOptions);
#else
    node.addClient<PahoSyncClient>(&clientOptions);
#endif

    node.enable();

    uint32_t retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        node.sync();
        sleep_for(milliseconds(50));
    }

    int32_t executeTime = 1000;

    while (true)
    {
        CPU::updateValues();

        node.execute(executeTime);

        sleep_for(milliseconds(executeTime));
    }
}