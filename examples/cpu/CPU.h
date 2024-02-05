/*
 * File: CPU.h
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

#ifndef EXAMPLES_CPU_CPU
#define EXAMPLES_CPU_CPU

#include <fstream>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <fstream>
#include <memory>
#include "Device.h"
#include "metrics/simple/DoubleMetric.h"

class CPU;

static std::vector<unique_ptr<CPU>> cpus;

/**
 * @brief Builds a list of Sparkplug Devices that can be filled with usage from the CPUs in the system
 *
 */
class CPU
{
private:
    uint64_t lastUser = 0;
    uint64_t lastUserLow = 0;
    uint64_t lastSys = 0;
    uint64_t lastIdle = 0;
    uint64_t lastIoWait = 0;
    uint64_t lastIrq = 0;
    uint64_t lastSoftIrq = 0;
    uint64_t lastTotal = 0;

    Device device;
    std::shared_ptr<DoubleMetric> usageMetric;
    std::shared_ptr<DoubleMetric> idleMetric;

    /**
     * @brief Calculates the CPU usage and updates the associated metrics
     *
     * @param totalUser
     * @param totalUserLow
     * @param totalSys
     * @param totalIdle
     * @param totalIoWait
     * @param totalIrq
     * @param totalSoftIrq
     */
    void update(uint64_t totalUser, uint64_t totalUserLow,
                uint64_t totalSys, uint64_t totalIdle,
                uint64_t totalIoWait, uint64_t totalIrq, uint64_t totalSoftIrq);

    /**
     * @brief Construct a new CPU containing a Device and usage Metrics
     *
     * @param name The name of the CPU, used as the Device name
     */
    CPU(const char *name);

    /**
     * @brief Get the Sparkplug Device assoeciated with the CPU
     *
     * @return Device*
     */
    Device *getDevice();

    /**
     * @brief Builds a list of CPUs based on data from /proc/stat
     *
     */
    static void buildCpuList();

protected:
public:
    /**
     * @brief Get the Sparkplug Devices of the CPUs
     *
     * @return std::vector<Device *>
     */
    static std::vector<Device *> getCpus();

    /**
     * @brief Updates the values of all CPUs
     *
     */
    static void updateValues();
};

#endif /* EXAMPLES_CPU_CPU */
