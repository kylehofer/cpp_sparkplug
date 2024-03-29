/*
 * File: CPU.cpp
 * Project: cpp_sparkplug
 * Created Date: Wednesday January 31st 2024
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

#include "CPU.h"

using namespace std;

void CPU::update(uint64_t currentUser, uint64_t currentUserLow,
                 uint64_t currentSys, uint64_t currentIdle,
                 uint64_t currentIoWait, uint64_t currentIrq,
                 uint64_t currentSoftIrq)
{

    double usagePercent, idlePercent;
    if (currentUser < lastUser || currentUserLow < lastUserLow ||
        currentSys < lastSys || currentIdle < lastIdle ||
        currentIoWait < lastIoWait || currentIrq < lastIrq ||
        currentSoftIrq < lastSoftIrq)
    {
        // Overflow detection. Just skip this value.
        usagePercent = 0.0;
        idlePercent = 0.0;
    }
    else
    {
        uint64_t total = (currentUser) +
                         (currentUserLow) +
                         (currentSys) +
                         (currentIdle) +
                         (currentIoWait) +
                         (currentIrq) +
                         (currentSoftIrq);
        double delta = total - lastTotal;
        double idle = currentIdle - lastIdle;
        double used = delta - idle;
        usagePercent = used * 100.0 / delta;
        idlePercent = 100.0 - usagePercent;

        lastTotal = total;
    }

    lastUser = currentUser;
    lastUserLow = currentUserLow;
    lastSys = currentSys;
    lastIdle = currentIdle;
    lastIoWait = currentIoWait;
    lastIrq = currentIrq;
    lastSoftIrq = currentSoftIrq;

    usageMetric->setValue(usagePercent);
    idleMetric->setValue(idlePercent);
}

Device *CPU::getDevice()
{
    return &device;
}

void CPU::buildCpuList()
{
    int count = 0;
    ifstream fileInput;
    fileInput.open("/proc/stat");
    string line;

    const char *search = "cpu";

    while (getline(fileInput, line))
    {
        if (line.find(search, 0) != string::npos)
        {
            count++;
        }
    }
    fileInput.close();

    char cpuName[9];

    for (uint8_t i = 0; i < count; i++)
    {
        if (i == 0)
        {
            sprintf(cpuName, "cpuTotal");
        }
        else
        {
            sprintf(cpuName, "cpu%u", i - 1);
        }
        cpus.push_back(unique_ptr<CPU>(new CPU(cpuName)));
    }
}

vector<Device *> CPU::getCpus()
{
    if (cpus.size() == 0)
    {
        buildCpuList();
    }

    vector<Device *> devices;

    for (auto &cpu : cpus)
    {
        devices.push_back(cpu->getDevice());
    }

    return devices;
}

void CPU::updateValues()
{
    uint64_t totalUser, totalUserLow, totalSys, totalIdle, totalIoWait, totalIrq, totalSoftIrq;

    int count = 0;
    ifstream fileInput;
    fileInput.open("/proc/stat");
    string line;

    const char *search = "cpu";

    char buffer[6];

    int index = 0;

    while (getline(fileInput, line))
    {
        if (line.find(search, 0) != string::npos)
        {
#if __GNUC__
#if __x86_64__ || __ppc64__
            sscanf(line.c_str(), "%s %lu %lu %lu %lu %lu %lu %lu", buffer, &totalUser, &totalUserLow,
                   &totalSys, &totalIdle, &totalIoWait, &totalIrq, &totalSoftIrq);
#else
            sscanf(line.c_str(), "%s %llu %llu %llu %llu %llu %llu %llu", buffer, &totalUser, &totalUserLow,
                   &totalSys, &totalIdle, &totalIoWait, &totalIrq, &totalSoftIrq);
#endif
#endif

            cpus.at(count)->update(totalUser, totalUserLow,
                                   totalSys, totalIdle,
                                   totalIoWait, totalIrq,
                                   totalSoftIrq);
            count++;

            index += 1;
        }
    }
    fileInput.close();
}
CPU::CPU(const char *name) : device(Device(name, 1000))
{
    usageMetric = DoubleMetric::create("usage", 0.00);
    idleMetric = DoubleMetric::create("idle", 0.00);

    device.addMetric(usageMetric);
    device.addMetric(idleMetric);
}
