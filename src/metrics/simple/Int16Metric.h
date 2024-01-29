/*
 * File: Int16Metric.h
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

#ifndef SRC_METRICS_SIMPLE_INT16METRIC
#define SRC_METRICS_SIMPLE_INT16METRIC

#include "SimpleMetric.h"
#include <stdint.h>
#include <tahu.h>

class Int16Metric : public SimpleMetric<int16_t>
{
private:
    /**
     * @brief Construct a new int16_t Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     */
    Int16Metric(const char *name, int16_t data) : SimpleMetric(name, data, METRIC_DATA_TYPE_INT16){};

public:
    /**
     * @brief Construct a new int16_t Sparkplug Metric shared pointer
     *
     * @param name The name of the Sparkplug Metric
     * @param data The first value of the metric
     * @return std::shared_ptr<Int16Metric>
     */
    static std::shared_ptr<Int16Metric> create(const char *name, int16_t data)
    {
        return std::shared_ptr<Int16Metric>(new Int16Metric(name, data));
    }
};

#endif /* SRC_METRICS_SIMPLE_INT16METRIC */
