/*
 * File: Publisher.h
 * Project: cpp_sparkplug
 * Created Date: Friday September 30th 2022
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

#ifndef INCLUDE_PUBLISHER
#define INCLUDE_PUBLISHER

#include "Metric.h"
#include <tahu.h>

/**
 * @brief An enum of all Publisher States
 * 
 */
enum PublisherState
{
    IDLE, CAN_PUBLISH, PUBLISHING
};

/**
 * @brief A class representing a Sparkplug Publisher (Node/Device)
 * This Class contains a set of Metrics and a configured Publish period that
 * determines the minimum time per publish.
 */
class Publisher
{
private:
    int32_t publishPeriod;
    int32_t nextPublish;
    PublisherState state;

protected:
    Metric **metrics;
    int16_t metricCount;
    /**
     * @brief Initializes a base payload for adding metrics to.
     * NOTE: Memory is allocated for the payload, and must be freed by the caller of this function.
     * 
     * @param isBirth 
     * @return org_eclipse_tahu_protobuf_Payload* 
     */
    org_eclipse_tahu_protobuf_Payload* initializePayload(bool isBirth);

public:
    /**
     * @brief Construct a new Publisher
     * 
     */
    Publisher();
    /**
     * @brief Construct a new Publisher
     * 
     * @param publishPeriod The minimum time required before sending messages
     */
    Publisher(int publishPeriod);
    /**
     * @brief Set the Metrics on the Publisher
     * 
     * @param metrics An array of Metrics that will be handled by this Publisher
     * @param metricCount The number of Metrics being added
     */
    void setMetrics(Metric** metrics, int16_t metricCount);
    /**
     * @brief Used to update the publishing timer for the Publisher. The amount of time supplied will be deducted from the remaining time before
     * the next publish. If more time has passed than the publish period then the Publisher will be marked as able to publish. The value returned
     * will be the remaining time before the next publish. If the Publisher is able to publish it will then return its publish period.
     * 
     * @param elapsed The amount of time to deduct from the remaining publishing time.
     * @return int32_t 
     */
    int32_t update(int32_t elapsed);
    /**
     * @brief Callback used to tell the Publisher that its publish request has been resolved by the broker.
     */
    void published();
    /**
     * @brief Whether the Publisher is available to publish
     * 
     * @return true 
     * @return false 
     */
    bool canPublish();
    /**
     * @brief Get the Sparkplug Payload that will be used to publish.
     * @param isBirth 
     * @return org_eclipse_tahu_protobuf_Payload* 
     */
    org_eclipse_tahu_protobuf_Payload* getPayload(bool isBirth = false);
    // void handleCommand(void* payload);
};

#endif /* INCLUDE_PUBLISHER */
