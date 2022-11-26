/*
 * File: Metric.h
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
 * OUT OF #include <tahu.h>OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * HISTORY:
 */

#ifndef INCLUDE_METRIC
#define INCLUDE_METRIC

#include <tahu.h>
// #include <tahu.pb.h>

class Metric
{
private:
    char* name;
    void* data;
    uint64_t alias;
    uint8_t dataType;
    size_t size;
    bool dirty;

    org_eclipse_tahu_protobuf_Payload_PropertySet* properties;
public:
    Metric();
    Metric(const char* name, void* data, size_t size, uint8_t dataType);
    ~Metric();
    template <typename T> Metric(const char *name, T data, uint8_t dataType) : Metric(name, &data, sizeof(T), dataType) {};
    void addToPayload(org_eclipse_tahu_protobuf_Payload* payload, bool isBirth = false);
    void setValue(void * data);
    bool isDirty();
    void published();
    void* getData();
};

#endif /* INCLUDE_METRIC */
