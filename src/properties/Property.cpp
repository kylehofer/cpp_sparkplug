/*
 * File: Property.cpp
 * Project: cpp_sparkplug
 * Created Date: Thursday January 25th 2024
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2024 Kyle Hofer
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

#include "Property.h"
#include <stdio.h>

Property::Property(const char *name, void *data, size_t size, uint8_t dataType)
{
    this->name = strdup(name);
    if (size > 0)
    {
        this->data = malloc(size);
        this->size = size;
        memcpy(this->data, data, size);
    }
    this->dataType = dataType;
}

Property::~Property()
{
    free(name);
    free(data);
}

Property &Property::operator=(const Property &right)
{
    if (&right != this)
    {
        if (data)
        {
            free(data);
        }

        if (name)
        {
            free(name);
        }

        if (right.data)
        {
            size = right.size;
            data = malloc(right.size);
            memcpy(data, right.data, size);
        }
        else
        {
            size = 0;
            data = nullptr;
        }

        if (right.name)
        {
            name = strdup(right.name);
        }
        else
        {
            name = nullptr;
        }

        dataType = right.dataType;
    }
    return *this;
}

Property &Property::operator=(Property right)
{
    if (&right != this)
    {
        if (data)
        {
            free(data);
        }

        if (name)
        {
            free(name);
        }

        if (right.data)
        {
            size = right.size;
            data = malloc(right.size);
            memcpy(data, right.data, size);
        }
        else
        {
            size = 0;
            data = nullptr;
        }

        if (right.name)
        {
            name = strdup(right.name);
        }
        else
        {
            name = nullptr;
        }

        dataType = right.dataType;
    }
    return *this;
}

int Property::addToPropertySet(org_eclipse_tahu_protobuf_Payload_PropertySet *propertySet, bool isBirth)
{
    if (dirty || isBirth)
    {
        return add_property_to_set(propertySet, name, dataType, data, size) == 0;
    }
    return 0;
}

void Property::setValue(void *data)
{
    (dirty = (dirty || memcmp(data, this->data, size) != 0)) && memcpy(this->data, data, size);
}

bool Property::isDirty()
{
    return dirty;
}

void Property::published()
{
    dirty = false;
}

void *Property::getData()
{
    return data;
}
const char *Property::getName()
{
    return name;
}
