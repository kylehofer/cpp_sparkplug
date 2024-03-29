/*
 * File: Publishable.h
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

#ifndef SRC_PUBLISHABLE
#define SRC_PUBLISHABLE

#include "metrics/Metric.h"
#include "CommonTypes.h"
#include "Publisher.h"
#include <tahu.h>
#include <forward_list>
#include <memory>

using namespace std;

class Publisher;

/**
 * @brief An enum of all Publishable States
 *
 */
enum PublishableState
{
    IDLE,
    CAN_PUBLISH,
    PUBLISHING
};

/**
 * @brief A class representing a Sparkplug Publishable (Node/Device)
 * This Class contains a set of Metrics and a configured Publish period that
 * determines the minimum time per publish.
 */
class Publishable
{
private:
    char *name = NULL;
    int32_t publishPeriod;
    int32_t nextPublish;
    PublishableState state = IDLE;

    forward_list<std::shared_ptr<Metric>> metrics;

    /**
     * @brief Get the State
     * Thread safe
     * @return PublishableState
     */
    PublishableState getState();
    /**
     * @brief Set the State
     * Thread safe
     */
    void setState(PublishableState);

protected:
    /**
     * @brief Set the Publish Period
     *
     * @param publishPeriod
     */
    void setPublishPeriod(int32_t publishPeriod);

public:
    ~Publishable();
    /**
     * @brief Construct a new Publishable
     *
     */
    Publishable();
    /**
     * @brief Construct a new Publishable
     *
     * @param publishPeriod The minimum time required before sending messages
     */
    Publishable(const char *name, int publishPeriod);
    /**
     * @brief Set the Metrics on the Publishable
     *
     * @param metrics An array of Metrics that will be handled by this Publishable
     * @param metricCount The number of Metrics being added
     */
    void addMetric(const std::shared_ptr<Metric> &metric);
    void addMetrics(const std::vector<std::shared_ptr<Metric>> &metrics);
    /**
     * @brief Used to update the publishing timer for the Publishable. The amount of time supplied will be deducted from the remaining time before
     * the next publish. If more time has passed than the publish period then the Publishable will be marked as able to publish. The value returned
     * will be the remaining time before the next publish. If the Publishable is able to publish it will then return its publish period.
     *
     * @param elapsed The amount of time to deduct from the remaining publishing time.
     * @return int32_t
     */
    int32_t update(int32_t elapsed);
    /**
     * @brief Callback used to tell the Publishable that its publish request has been resolved by the client.
     */
    void published();
    /**
     * @brief Callback used to tell the Publishable that its being published.
     */
    void publishing();
    /**
     * @brief Whether the Publishable is available to publish
     *
     * @return true
     * @return false
     */
    bool canPublish();
    /**
     * @brief Adds the metrics from a publisher to a protobuf payload
     *
     * @param payload A protobuf payload that the Metrics will be added to
     * @param isBirth If the payload is a part of a birth message
     */
    void addToPayload(org_eclipse_tahu_protobuf_Payload *payload, bool isBirth = false);
    /**
     * @brief Get the name of the Publishable
     *
     * @return const char*
     */
    const char *getName();

    /**
     * @brief Callback for when a command has been received for the publishable.
     *
     * @param publisher The publisher that the command was received from
     * @param message The message received
     */
    void handleCommand(Publisher *publisher, const void *payload, const int payloadLength);

    /**
     * @brief Returns whether the Publishable instance is a Node.
     *
     * @return true
     * @return false
     */
    virtual bool isNode() = 0;
};

#endif /* SRC_PUBLISHABLE */
