/*
 * File: Publisher.cpp
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

#include "Publisher.h"
#include <iostream>

using namespace std;

Publisher::Publisher() : Publisher(30) { }

Publisher::Publisher(int publishPeriod) : publishPeriod(publishPeriod), nextPublish(publishPeriod), state(IDLE), metricCount(0) { }

void Publisher::setMetrics(Metric** metrics, int16_t metricCount)
{
    this->metrics = metrics;
    this->metricCount = metricCount;
}

int32_t Publisher::update(int32_t elapsed)
{
    if (state == PUBLISHING || state == CAN_PUBLISH)
    {
        return nextPublish;
    }

    nextPublish -= elapsed;

    if (nextPublish <= 0)
    {
        state = CAN_PUBLISH;
        nextPublish = publishPeriod;
    }

    return nextPublish;
}

void Publisher::published()
{
    state = IDLE;
    for (int16_t i = (metricCount - 1); i >= 0; i--)
    {
        metrics[i]->published();
    }
}

org_eclipse_tahu_protobuf_Payload* Publisher::initializePayload(bool isBirth)
{
    org_eclipse_tahu_protobuf_Payload* payload = (org_eclipse_tahu_protobuf_Payload*) malloc(sizeof (org_eclipse_tahu_protobuf_Payload));
    get_next_payload(payload);
    nextPublish = publishPeriod;
    return payload;
}

org_eclipse_tahu_protobuf_Payload* Publisher::getPayload(bool isBirth)
{
    if (!canPublish() && !isBirth)
    {
        return NULL;
    }

    org_eclipse_tahu_protobuf_Payload* payload = initializePayload(isBirth);

    state = PUBLISHING;

    nextPublish = publishPeriod;

    for (int16_t i = (metricCount - 1); i >= 0; i--)
    {
        metrics[i]->addToPayload(payload, isBirth);
    }

    return payload;
}

bool Publisher::canPublish()
{
    if (state != CAN_PUBLISH)
    {
        return false;
    }
    for (int16_t i = (metricCount - 1); i >= 0; i--)
    {
        if (metrics[i]->isDirty()) {
            return true;
        }
    }
    return false;
}

