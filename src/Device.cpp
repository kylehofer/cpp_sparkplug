/*
 * File: Device.cpp
 * Project: sparkplug_c
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

#include "Device.h"

/**
 * @brief Helper function to add device controls to a payload
 * 
 * @param payload 
 * @return org_eclipse_tahu_protobuf_Payload* 
 */
static org_eclipse_tahu_protobuf_Payload* addDeviceControl(org_eclipse_tahu_protobuf_Payload* payload)
{
    bool rebirth_value = false;
    add_simple_metric(payload, "Device Control/Rebirth", false, 0, METRIC_DATA_TYPE_BOOLEAN, false, false, &rebirth_value, sizeof(rebirth_value));
    return payload;
}

Device::Device() : Publisher(), name(NULL) { }

Device::Device(const char* name, int publishPeriod) : Publisher(publishPeriod), name(name) { }

org_eclipse_tahu_protobuf_Payload* Device::initializePayload(bool isBirth)
{
    return isBirth ? addDeviceControl(Publisher::initializePayload(isBirth)) : Publisher::initializePayload(isBirth);
}

const char* Device::getName()
{
    return name;
}

