/*
 * File: Publisher.h
 * Project: sparkplug_c
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

enum PublisherState
{
    IDLE, CAN_PUBLISH, PUBLISHING
};

class Publisher
{
private:
    int32_t publishPeriod;
    int32_t nextPublish;
    PublisherState state;

protected:
    Metric **metrics;
    int16_t metricCount;
    org_eclipse_tahu_protobuf_Payload* initializePayload(bool isBirth);

public:
    Publisher();
    Publisher(int publishPeriod);
    void setMetrics(Metric** metrics, int16_t metricCount);
    int32_t update(int32_t elapsed);
    int32_t update(int32_t elapsed, org_eclipse_tahu_protobuf_Payload** payload);
    void published();
    bool canPublish();
    // void handleCommand(void* payload);
    org_eclipse_tahu_protobuf_Payload* getPayload(bool isBirth = false);
};

#endif /* INCLUDE_PUBLISHER */
