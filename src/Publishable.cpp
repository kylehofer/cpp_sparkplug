/*
 * File: Publishable.cpp
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

#include "Publishable.h"
#include <algorithm>
#include <cstdio>

#ifdef DEBUGGING
#define LOGGER(format, ...)  \
    printf("Publishable: "); \
    printf(format, ##__VA_ARGS__)
#else
#define LOGGER(out, ...)
#endif

using namespace std;

Publishable::~Publishable()
{
    if (this->name != NULL)
    {
        free(this->name);
    }
}

Publishable::Publishable() : Publishable(NULL, 30) {}

Publishable::Publishable(const char *name, int publishPeriod) : publishPeriod(publishPeriod), nextPublish(publishPeriod)
{

    if (name != NULL)
    {
        this->name = strdup(name);
    }
}

void Publishable::addMetric(const std::shared_ptr<Metric> &metric)
{
    metrics.push_front(std::move(metric));
}

void Publishable::addMetrics(const std::vector<std::shared_ptr<Metric>> &metrics)
{
    for (auto &metric : metrics)
    {
        addMetric(metric);
    }
}

int32_t Publishable::update(int32_t elapsed)
{
    PublishableState state = getState();
    if (state == PUBLISHING || state == CAN_PUBLISH)
    {
        return publishPeriod;
    }

    nextPublish -= elapsed;

    if (nextPublish <= 0)
    {
        setState(CAN_PUBLISH);
        nextPublish = publishPeriod;
    }

    return nextPublish;
}

void Publishable::setPublishPeriod(int32_t publishPeriod)
{
    this->publishPeriod = publishPeriod;
    nextPublish = publishPeriod;
}

PublishableState Publishable::getState()
{
    return state;
}

void Publishable::setState(PublishableState state)
{
    this->state = state;
}

void Publishable::published()
{
    setState(IDLE);
    std::for_each(metrics.begin(), metrics.end(), [](std::shared_ptr<Metric> &metric)
                  { metric->published(); });
}

void Publishable::publishing()
{
    setState(PUBLISHING);
}

void Publishable::addToPayload(org_eclipse_tahu_protobuf_Payload *payload, bool isBirth)
{
    nextPublish = publishPeriod;
    for_each(metrics.begin(), metrics.end(), [payload, isBirth](std::shared_ptr<Metric> &metric)
             { metric->addToPayload(payload, isBirth); });
}

bool Publishable::canPublish()
{
    if (getState() != CAN_PUBLISH)
    {
        return false;
    }
    return any_of(metrics.begin(), metrics.end(), [](std::shared_ptr<Metric> &metric)
                  { return metric->isDirty(); });
}

const char *Publishable::getName()
{
    return name;
}

void Publishable::handleCommand(__attribute__((unused)) Publisher *publisher, const void *payload, const int payloadLength)
{
    // Decode the payload
    org_eclipse_tahu_protobuf_Payload sparkplugPayload;
    if (decode_payload(&sparkplugPayload, (uint8_t *)payload, payloadLength) < 0)
    {
        free_payload(&sparkplugPayload);
        return;
    }

    for (int i = sparkplugPayload.metrics_count - 1; i >= 0; i--)
    {
        org_eclipse_tahu_protobuf_Payload_Metric metricPayload = sparkplugPayload.metrics[i];
        char *name = metricPayload.name;
        forward_list<std::shared_ptr<Metric>>::iterator result;

        result = find_if(metrics.begin(), metrics.end(), [name](std::shared_ptr<Metric> &metric)
                         { return (strcmp(metric->getName(), name) == 0); });

        if (result != metrics.end())
        {
            std::shared_ptr<Metric> metric;
            metric = *result;
            // Handle Device Command
            metric->onCommand(&metricPayload);
        }
    }

    free_payload(&sparkplugPayload);
}
