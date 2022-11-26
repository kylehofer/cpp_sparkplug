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
#include "Metric.h"
#include <iostream>

Metric::~Metric()
{
    free(name);
    free(data);
}

Metric::Metric(const char *name, void *data, size_t size, uint8_t dataType) : dataType(dataType), dirty(false)
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
        if (add_simple_metric(payload, name, true, alias, dataType, false, false, data, size) < 0)
        {
            // TODO: failure
        }
    }
}

void Metric::setValue(void *data)
{
    (dirty = (dirty || memcmp(data, this->data, size) != 0)) && memcpy(this->data, data, size);
};

bool Metric::isDirty()
{
    return dirty;
}

void Metric::published()
{
    dirty = false;
}

void* Metric::getData()
{
    return data;
}