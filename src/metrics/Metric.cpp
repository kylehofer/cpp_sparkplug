/*
 * File: Metric.cpp
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

#include <utility>

#include "Metric.h"
#include "properties/simple/BooleanProperty.h"
#include <pb_decode.h>
#include <iostream>

Metric::~Metric()
{
    free(name);
    free(data);
}

Metric::Metric(const char *name, void *data, size_t size, uint8_t dataType) : dataType(dataType)
{
    this->name = strdup(name);
    this->data = malloc(size);
    this->size = size;
    memcpy(this->data, data, size);
}

void Metric::addToPayload(org_eclipse_tahu_protobuf_Payload *payload, bool isBirth)
{
    if (dirty || isBirth)
    {
        org_eclipse_tahu_protobuf_Payload_Metric metric;
        if (init_metric(&metric, name, true, alias, dataType, false, false, data, size) != 0)
        {
        }

        metric.has_timestamp = true;
        metric.timestamp = isBirth ? TimeManager::getTime() : changedTime;

        if (properties.size() > 0)
        {
            org_eclipse_tahu_protobuf_Payload_PropertySet propertySet;
            memset(&propertySet, 0, sizeof(propertySet));

            bool added = false;

            for (auto &property : properties)
            {
                added |= property->addToPropertySet(&propertySet, isBirth);
            }
            if (added)
            {
                add_propertyset_to_metric(&metric, &propertySet);
            }
        }

        if (add_metric_to_payload(payload, &metric) < 0)
        {
            // TODO: failure
        }
    }
}

void Metric::setValue(void *data)
{
    if (dirty || memcmp(data, this->data, size) != 0)
    {
        dirty = true;
        memcpy(this->data, data, size);
        changedTime = TimeManager::getTime();
    }
};

bool Metric::isDirty()
{
    return dirty;
}

void Metric::published()
{
    dirty = false;
    if (properties.size() > 0)
    {

        for (auto &property : properties)
        {
            property->published();
        }
    }
}

void *Metric::getData()
{
    return data;
}

const char *Metric::getName()
{
    return name;
}

void Metric::addProperty(const std::shared_ptr<Property> &property)
{
    properties.push_back(std::move(property));
}

void Metric::addProperties(const std::vector<std::shared_ptr<Property>> &properties)
{
    for (auto property : properties)
    {
        addProperty(property);
    }
}

void Metric::setCommandHandler(CommandHandler *handler)
{
    if (isReadOnly)
    {
        isReadOnly = false;
        addProperty(BooleanProperty::create("writable", true));
    }
    // Add read only property
    this->handler = handler;
}

void Metric::setCommandCallback(std::function<void(Metric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload)> callback)
{
    if (isReadOnly)
    {
        isReadOnly = false;
        addProperty(BooleanProperty::create("writable", true));
    }
    // Add read only property
    this->callback = callback;
}

void Metric::onCommand(org_eclipse_tahu_protobuf_Payload_Metric *metric)
{
    if (handler != nullptr)
    {
        handler->onMetricCommand(this, metric);
    }

    if (callback)
    {
        callback(this, metric);
    }
};