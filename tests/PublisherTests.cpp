/*
 * File: PublishableTests.cpp
 * Project: cpp_sparkplug
 * Created Date: Thursday November 24th 2022
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

#include "gtest/gtest.h"

#include <tahu.h>

#include "Device.h"
#include "metrics/simple/Int32Metric.h"

TEST(Publishable, TestUpdate)
{
    Device testPublishable = Device("name", 30);

    EXPECT_EQ(testPublishable.update(5), 25);
    EXPECT_EQ(testPublishable.update(0), 25);
    EXPECT_EQ(testPublishable.update(10), 15);
    EXPECT_EQ(testPublishable.update(15), 30);
    EXPECT_EQ(testPublishable.update(5), 30);

    testPublishable.published();

    EXPECT_EQ(testPublishable.update(10), 20);
    EXPECT_EQ(testPublishable.update(19), 1);
    EXPECT_EQ(testPublishable.update(1000), 30);

    testPublishable.published();

    EXPECT_EQ(testPublishable.update(5), 25);

    testPublishable.publishing();

    EXPECT_EQ(testPublishable.update(5), 30);
}

TEST(Publishable, TestCanPublish)
{
    Device testPublishable = Device("name", 30);
    auto testMetric = Int32Metric::create("MetricName", 20);

    testPublishable.addMetric(testMetric);

    EXPECT_EQ(testPublishable.update(30), 30);
    EXPECT_FALSE(testPublishable.canPublish());

    int newValue = 21;
    testMetric->setValue(newValue);
    EXPECT_TRUE(testPublishable.canPublish());

    testPublishable.published();

    newValue = 22;
    testMetric->setValue(newValue);
    EXPECT_FALSE(testPublishable.canPublish());

    EXPECT_EQ(testPublishable.update(30), 30);
    EXPECT_TRUE(testPublishable.canPublish());
}