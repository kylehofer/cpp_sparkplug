/*
 * File: SimpleMetric.h
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

#ifndef SRC_METRICS_SIMPLE_SIMPLEMETRIC
#define SRC_METRICS_SIMPLE_SIMPLEMETRIC

#include "../Metric.h"
#include <memory>

/**
 * @brief Simple Metric implementation.
 * Does not react to commands.
 *
 */
template <typename T>
class SimpleMetric : public Metric
{
private:
public:
    /**
     * @brief Construct a new Sparkplug Metric
     *
     */
    SimpleMetric() : Metric(){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data A pointer to a piece of data that will be as the first value of the metric
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    SimpleMetric(const char *name, void *data, size_t size, uint8_t dataType) : Metric(name, data, size, dataType){};
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @tparam T The data type that will be used to construct the data. Does not work with pointers.
     * @param name The name of the Sparkplug Metric
     * @param data The piece of data that will be as the first value of the metric
     * @param dataType Sparkplug Datatype
     */
    SimpleMetric(const char *name, T data, uint8_t dataType) : Metric(name, &data, sizeof(T), dataType){};

    inline static SimpleMetric<uint8_t> create(const char *name, uint8_t value)
    {
        return SimpleMetric<uint8_t>(name, value, METRIC_DATA_TYPE_UINT8);
    }

    /**
     * @brief Sets a new value of the metric
     *
     * @param data Pointer to the a piece of data that will be copied to the metric
     */
    void setValue(T value)
    {
        Metric::setValue(&value);
    };

    T &getValue()
    {
        return *(T *)data;
    };
};

#endif /* SRC_METRICS_SIMPLE_SIMPLEMETRIC */
