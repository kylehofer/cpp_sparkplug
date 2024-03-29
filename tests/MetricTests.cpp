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

#include "utils/MockTimeManager.h"

#include "metrics/simple/Int32Metric.h"
#include "metrics/simple/StringMetric.h"

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

TEST(SimpleMetric, TestStringDirty)
{
    auto testMetric = StringMetric::create("MetricName", "SomeString");

    EXPECT_FALSE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), "SomeString");

    testMetric->setValue("SomeString");
    EXPECT_FALSE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), "SomeString");

    testMetric->setValue("AnotherString");
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), "AnotherString");
    EXPECT_NE(testMetric->getValue(), "SomeString");

    testMetric->setValue("AnotherString");
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), "AnotherString");
    EXPECT_NE(testMetric->getValue(), "SomeString");

    testMetric->setValue("SomeString");
    EXPECT_TRUE(testMetric->isDirty());
    EXPECT_EQ(testMetric->getValue(), "SomeString");
    EXPECT_NE(testMetric->getValue(), "AnotherString");
}

TEST(SimpleMetric, TestAddToPayload)
{
    MockTimeManager mockManager;
    TimeManager::setInstance((TimeClient *)&mockManager);
    char metricName[] = {"MetricName"};
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    auto testMetric = Int32Metric::create("MetricName", 20);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric->addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    EXPECT_EQ(payload.metrics[0].value.int_value, 20);
    EXPECT_TRUE(payload.metrics[0].has_timestamp);
    EXPECT_EQ(payload.metrics[0].timestamp, 0);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    int newValue = 50;

    mockManager.setTime(1000);
    testMetric->setValue(newValue);

    mockManager.setTime(2000);
    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_EQ(payload.metrics[1].value.int_value, 50);
    EXPECT_TRUE(payload.metrics[1].has_timestamp);
    EXPECT_EQ(payload.metrics[1].timestamp, 1000);

    EXPECT_TRUE(testMetric->isDirty());

    testMetric->published();

    EXPECT_FALSE(testMetric->isDirty());

    TimeManager::reset();

    free_payload(&payload);
}