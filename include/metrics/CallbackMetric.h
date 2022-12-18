/*
 * File: CallbackMetric.h
 * Project: sparkplug_c
 * Created Date: Sunday December 4th 2022
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

#ifndef INCLUDE_METRICS_CALLBACKMETRIC
#define INCLUDE_METRICS_CALLBACKMETRIC

#include <tahu.h>
#include <stdlib.h>
#include <functional>
#include "CommonTypes.h"
#include "metrics/Metric.h"

class CallbackMetric;

/**
 * @brief Class for handling when a Metric has recieved a command.
 * 
 */
class CommandHandler
{
public:
    virtual void onMetricCommand(CallbackMetric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload) = 0;
};

/**
 * @brief This class represents a Metric that can fire callbacks when a command is received by a metric.
 * 
 */
class CallbackMetric : Metric
{
private:
    CommandHandler *handler = NULL;
    function<void(CallbackMetric *, org_eclipse_tahu_protobuf_Payload_Metric *)> callback;

protected:
public:
    /**
     * @brief Construct a new Sparkplug Metric
     *
     */
    CallbackMetric() : Metric(){};
    ~CallbackMetric(){};
    /**
     * @brief Construct a new Callback Metric
     *
     * @param name The name of the Metric
     * @param data A pointer to a piece of data that will be as the first value of the metric
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    CallbackMetric(const char *name, void *data, size_t size, uint8_t dataType, CommandHandler *handler) : Metric(name, data, size, dataType), handler(handler){};
    /**
     * @brief Construct a new Callback Metric
     *
     * @tparam T The data type that will be used to construct the data. Does not work with pointers.
     * @param name The name of the Metric
     * @param data The piece of data that will be as the first value of the metric
     * @param dataType Sparkplug Datatype
     */
    template <typename T>
    CallbackMetric(const char *name, T data, uint8_t dataType, CommandHandler *handler) : Metric(name, &data, sizeof(T), dataType), handler(handler){};

    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @tparam T The data type that will be used to construct the data. Does not work with pointers.
     * @param name The name of the Sparkplug Metric
     * @param data The piece of data that will be as the first value of the metric
     * @param dataType Sparkplug Datatype
     */
    template <typename T>
    CallbackMetric(const char *name, T data, uint8_t dataType, function<void(CallbackMetric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload)> callback) : Metric(name, &data, sizeof(T), dataType), callback(callback){};

    /**
     * @brief Sets the command handler for the metric.
     * This handler have onMetricCommand called when a command is received for this metric.
     * 
     * @param handler 
     */
    void setCommandHandler(CommandHandler *handler)
    {
        this->handler = handler;
    }

    /**
     * @brief Set the command callback for this metric.
     * This callback will be executed when a command is received for this metric.
     * 
     * @param callback 
     */
    void setCommandCallback(function<void(CallbackMetric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload)> callback)
    {
        this->callback = callback;
    }

    /**
     * @brief Informs the Command metric a Payload Metric has been received for this Metric.
     * 
     * @param metric 
     */
    void onCommand(org_eclipse_tahu_protobuf_Payload_Metric *metric)
    {
        if (handler != NULL)
        {
            handler->onMetricCommand(this, metric);
        }

        if (callback != NULL)
        {
            callback(this, metric);
        }
    };
};

#endif /* INCLUDE_METRICS_CALLBACKMETRIC */
