/*
 * File: CommandMetric.h
 * Project: sparkplug_c
 * Created Date: Thursday December 1st 2022
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

#ifndef INCLUDE_METRICS_COMMANDMETRIC
#define INCLUDE_METRICS_COMMANDMETRIC

#include "CommonTypes.h"
#include "Metrics/Metric.h"

class CommandMetric : Metric
{
private:
protected:
    const bool command = false;

public:
    /**
     * @brief Construct a new Sparkplug Metric
     *
     */
    CommandMetric() : Metric(){};
    ~CommandMetric(){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data A pointer to a piece of data that will be as the first value of the metric
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    CommandMetric(const char *name, void *data, size_t size, uint8_t dataType) : Metric(name, data, size, dataType){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @tparam T The data type that will be used to construct the data. Does not work with pointers.
     * @param name The name of the Sparkplug Metric
     * @param data The piece of data that will be as the first value of the metric
     * @param dataType Sparkplug Datatype
     */
    template <typename T>
    CommandMetric(const char *name, T data, uint8_t dataType) : Metric(name, &data, sizeof(T), dataType){};

    virtual void onCommand(org_eclipse_tahu_protobuf_Payload_Metric *payload) = 0;
};

#endif /* INCLUDE_METRICS_COMMANDMETRIC */
