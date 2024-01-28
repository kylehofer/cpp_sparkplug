/*
 * File: Metric.h
 * Project: cpp_sparkplug
 * Created Date: Sunday September 25th 2022
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

#ifndef SRC_METRICS_METRIC
#define SRC_METRICS_METRIC

#include <tahu.h>
#include <vector>
#include <functional>
#include <memory>
#include "../properties/Property.h"

class Metric;

/**
 * @brief Class for handling when a Metric has recieved a command.
 *
 */
class CommandHandler
{
public:
    virtual void onMetricCommand(Metric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload) = 0;
};

/**
 * @brief
 *
 */
class Metric
{
private:
    char *name = NULL;

    uint64_t alias = 0;
    uint8_t dataType;
    size_t size;
    bool dirty = false;

    CommandHandler *handler = NULL;
    std::function<void(Metric *, org_eclipse_tahu_protobuf_Payload_Metric *)> callback;
    bool isReadOnly = true;

protected:
    std::vector<std::shared_ptr<Property>> properties;
    void *data = NULL;

public:
    /**
     * @brief Construct a new Sparkplug Metric
     *
     */
    Metric();
    /**
     * @brief Construct a new Sparkplug Metric
     *
     * @param name The name of the Sparkplug Metric
     * @param data A pointer to a piece of data that will be as the first value of the metric
     * @param size The memory size required for the data
     * @param dataType Sparkplug Datatype
     */
    Metric(const char *name, void *data, size_t size, uint8_t dataType);
    virtual ~Metric();
    /**
     * @brief Adds the metric to a protobuf payload
     *
     * @param payload A protobuf payload that the Sparkplug Metric will be added to
     * @param isBirth If the payload is a part of a birth message
     */
    void addToPayload(org_eclipse_tahu_protobuf_Payload *payload, bool isBirth = false);
    /**
     * @brief Sets a new value of the metric
     *
     * @param data Pointer to the a piece of data that will be copied to the metric
     */
    void setValue(void *data);
    /**
     * @brief Whether the metric value is dirty, and can be published
     *
     * @return true
     * @return false
     */
    bool isDirty();
    /**
     * @brief Used to mark the metric that is had been published
     *
     */
    void published();
    /**
     * @brief Returns the pointer to the metric data
     *
     * @return void*
     */
    void *getData();
    /**
     * @brief Returns the pointer to the metric data
     *
     * @return void*
     */
    const char *getName();

    /**
     * @brief Fired when a command is received for this Metric.
     *
     * @param metric
     */
    void onCommand(org_eclipse_tahu_protobuf_Payload_Metric *metric);

    void addProperty(const std::shared_ptr<Property> &property);

    /**
     * @brief Sets the command handler for the metric.
     * This handler have onMetricCommand called when a command is received for this metric.
     *
     * @param handler
     */
    void setCommandHandler(CommandHandler *handler);

    /**
     * @brief Set the command callback for this metric.
     * This callback will be executed when a command is received for this metric.
     *
     * @param callback
     */
    void setCommandCallback(std::function<void(Metric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload)> callback);
};

#endif /* SRC_METRICS_METRIC */
