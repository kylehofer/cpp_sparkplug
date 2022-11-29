/*
 * File: Device.h
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

#ifndef INCLUDE_DEVICE
#define INCLUDE_DEVICE

#include "Publisher.h"

/**
 * @brief Class that represents a Sparkplug Device.
 * Can hold a set of Metrics that can be published as a Sparkplug Device.
 */
class Device : Publisher
{
private:
    const char* name;
protected:
    /**
     * @brief Initializes a base payload for adding metrics to.
     * Will add Command metrics if it is a Birth payload.
     * NOTE: Memory is allocated for the payload, and must be freed by the caller of this function.
     * 
     * @param isBirth 
     * @return org_eclipse_tahu_protobuf_Payload* 
     */
    org_eclipse_tahu_protobuf_Payload* initializePayload(bool isBirth);
public:
    /**
     * @brief Construct a new Sparkplug Device
     * 
     */
    Device();
    /**
     * @brief Construct a new Sparkplug Device
     * 
     * @param name The name of the device
     * @param publishPeriod The minimum time required between each publish.
     */
    Device(const char* name, int publishPeriod);
    /**
     * @brief Get the name of the Device
     * 
     * @return const char* 
     */
    const char* getName();
    using Publisher::canPublish;
    using Publisher::update;
    using Publisher::getPayload;
    using Publisher::setMetrics;
};

#endif /* INCLUDE_DEVICE */
