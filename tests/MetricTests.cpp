/*
 * File: MetricTests.cpp
 * Project: cpp_sparkplug
 * Created Date: Sunday January 28th 2024
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

#include "gtest/gtest.h"

#include <tahu.h>

#include "metrics/simple/Int32Metric.h"

typedef struct
{
    int property1;
    int property2;
} TestStruct;

TEST(SimpleMetric, TestSimpleDirty)
{
    auto testMetric = Int32Metric::create("MetricName", 20);

    EXPECT_FALSE(testMetric->isDirty());

    int newValue;

    newValue = 20;

    testMetric->setValue(newValue);
    EXPECT_FALSE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), newValue);

    newValue = 21;

    testMetric->setValue(newValue);
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), newValue);

    newValue = 21;

    testMetric->setValue(newValue);
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), newValue);

    newValue = 20;

    testMetric->setValue(newValue);
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), newValue);
}

// TEST(SimpleMetric, TestStringDirty)
// {
//     static const int BUFFER_SIZE = 20;

//     char buffer1[BUFFER_SIZE];
//     char buffer2[BUFFER_SIZE];

//     memset(buffer1, 0, BUFFER_SIZE);
//     memset(buffer2, 0, BUFFER_SIZE);

//     memcpy(buffer1, "SomeString", 11);

//     SimpleMetric testMetric = SimpleMetric("MetricName", buffer1, BUFFER_SIZE, METRIC_DATA_TYPE_STRING);

//     EXPECT_FALSE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), buffer1, BUFFER_SIZE), 0);
//     EXPECT_NE(testMetric->getValue(), buffer1);

//     memcpy(buffer2, "SomeString", 11);

//     testMetric->setValue(buffer2);
//     EXPECT_FALSE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), buffer2, BUFFER_SIZE), 0);
//     EXPECT_NE(testMetric->getValue(), buffer2);

//     memcpy(buffer2, "AnotherString", 11);

//     testMetric->setValue(buffer2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), buffer2, BUFFER_SIZE), 0);
//     EXPECT_NE(testMetric->getValue(), buffer2);

//     memcpy(buffer2, "AnotherString", 11);

//     testMetric->setValue(buffer2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), buffer2, BUFFER_SIZE), 0);
//     EXPECT_NE(testMetric->getValue(), buffer2);

//     memcpy(buffer2, "SomeString", 11);

//     testMetric->setValue(buffer2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), buffer2, BUFFER_SIZE), 0);
//     EXPECT_NE(testMetric->getValue(), buffer2);
// }

// TEST(SimpleMetric, TestStructDirty)
// {
//     TestStruct value1 = {5, 10};
//     TestStruct value2 = {5, 10};

//     static const int STRUCT_SIZE = sizeof(TestStruct);

//     SimpleMetric testMetric = SimpleMetric("MetricName", value1, METRIC_DATA_TYPE_INT32);

//     EXPECT_FALSE(testMetric->isDirty());

//     testMetric->setValue(&value2);
//     EXPECT_FALSE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value1, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value1);

//     value2 = {10, 5};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);

//     value2 = {10, 5};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);

//     value2 = {5, 10};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);
// }

// TEST(SimpleMetric, TestStructPointerDirty)
// {
//     TestStruct value1 = {5, 10};
//     TestStruct value2 = {5, 10};

//     static const int STRUCT_SIZE = sizeof(TestStruct);

//     SimpleMetric testMetric = SimpleMetric("MetricName", &value1, STRUCT_SIZE, METRIC_DATA_TYPE_INT32);

//     EXPECT_FALSE(testMetric->isDirty());

//     testMetric->setValue(&value2);
//     EXPECT_FALSE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value1, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value1);

//     value2 = {10, 5};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);

//     value2 = {10, 5};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);

//     value2 = {5, 10};

//     testMetric->setValue(&value2);
//     EXPECT_TRUE(testMetric->isDirty());
//     EXPECT_EQ(memcmp(testMetric->getValue(), &value2, STRUCT_SIZE), 0);
//     EXPECT_NE((TestStruct *)testMetric->getValue(), &value2);
// }

TEST(SimpleMetric, TestAddToPayload)
{
    char metricName[] = {"MetricName"};
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    auto testMetric = Int32Metric::create("MetricName", 20);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric->addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    EXPECT_EQ(payload.metrics[0].value.int_value, 20);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    int newValue = 50;

    testMetric->setValue(newValue);
    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_EQ(payload.metrics[1].value.int_value, 50);

    EXPECT_TRUE(testMetric->isDirty());

    testMetric->published();

    EXPECT_FALSE(testMetric->isDirty());

    free_payload(&payload);
}