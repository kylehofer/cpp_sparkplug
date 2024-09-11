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

#include "properties/simple/UInt8Property.h"
#include "properties/simple/StringProperty.h"
#include "properties/complex/PropertySet.h"

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

TEST(SimpleMetric, TestAddIntToPayload)
{
    MockTimeManager mockManager;
    TimeManager::setInstance((TimeClient *)&mockManager);
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    auto testMetric = Int32Metric::create("MetricName", 20);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric->addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    EXPECT_STREQ(payload.metrics[0].name, "MetricName");
    EXPECT_EQ(payload.metrics[0].value.int_value, 20);
    EXPECT_TRUE(payload.metrics[0].has_timestamp);
    EXPECT_EQ(payload.metrics[0].timestamp, 0);
    EXPECT_FALSE(payload.metrics[0].has_properties);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    int newValue = 50;

    mockManager.setTime(1000);
    testMetric->setValue(newValue);

    mockManager.setTime(2000);
    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_STREQ(payload.metrics[1].name, "MetricName");
    EXPECT_EQ(payload.metrics[1].value.int_value, 50);
    EXPECT_TRUE(payload.metrics[1].has_timestamp);
    EXPECT_EQ(payload.metrics[1].timestamp, 1000);
    EXPECT_FALSE(payload.metrics[1].has_properties);

    EXPECT_TRUE(testMetric->isDirty());

    testMetric->published();

    EXPECT_FALSE(testMetric->isDirty());

    TimeManager::reset();

    free_payload(&payload);
}

TEST(SimpleMetric, TestAddStringToPayload)
{
    MockTimeManager mockManager;
    TimeManager::setInstance((TimeClient *)&mockManager);
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    auto testMetric = StringMetric::create("MetricName", "MyString");

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric->addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    EXPECT_STREQ(payload.metrics[0].name, "MetricName");
    EXPECT_STREQ(payload.metrics[0].value.string_value, "MyString");
    EXPECT_TRUE(payload.metrics[0].has_timestamp);
    EXPECT_EQ(payload.metrics[0].timestamp, 0);
    EXPECT_FALSE(payload.metrics[0].has_properties);

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    mockManager.setTime(1000);
    testMetric->setValue("Another String");

    mockManager.setTime(2000);
    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_STREQ(payload.metrics[1].name, "MetricName");
    EXPECT_STREQ(payload.metrics[1].value.string_value, "Another String");
    EXPECT_TRUE(payload.metrics[1].has_timestamp);
    EXPECT_EQ(payload.metrics[1].timestamp, 1000);
    EXPECT_FALSE(payload.metrics[1].has_properties);

    EXPECT_TRUE(testMetric->isDirty());

    testMetric->published();

    EXPECT_FALSE(testMetric->isDirty());

    TimeManager::reset();

    free_payload(&payload);
}

TEST(SimpleMetric, TestAddToPayloadWithProperties)
{
    MockTimeManager mockManager;
    TimeManager::setInstance((TimeClient *)&mockManager);
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    auto testMetric = StringMetric::create("MetricName", "MyString");

    testMetric->addProperties({
        PropertySet::create("enum",
                            {
                                UInt8Property::create("OFF", 0),
                                UInt8Property::create("FAULT", 2),
                            }),
        StringProperty::create("trueText", "Active"),
        StringProperty::create("falseTest", "Deactive"),
    });

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric->addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    auto metric = payload.metrics[0];
    EXPECT_STREQ(metric.name, "MetricName");
    EXPECT_STREQ(metric.value.string_value, "MyString");
    EXPECT_TRUE(metric.has_timestamp);
    EXPECT_EQ(metric.timestamp, 0);
    EXPECT_TRUE(metric.has_properties);

    EXPECT_EQ(metric.properties.keys_count, 3);
    EXPECT_STREQ(metric.properties.keys[0], "enum");
    EXPECT_STREQ(metric.properties.keys[1], "trueText");
    EXPECT_STREQ(metric.properties.keys[2], "falseTest");

    EXPECT_EQ(metric.properties.values_count, 3);

    EXPECT_EQ(metric.properties.values[0].type, PROPERTY_DATA_TYPE_PROPERTYSET);

    auto propertySet = metric.properties.values[0].value.propertyset_value;
    EXPECT_EQ(propertySet.keys_count, 2);
    EXPECT_EQ(propertySet.values_count, 2);
    EXPECT_STREQ(propertySet.keys[0], "OFF");
    EXPECT_STREQ(propertySet.keys[1], "FAULT");
    EXPECT_EQ(propertySet.values[0].value.int_value, 0);
    EXPECT_EQ(propertySet.values[1].value.int_value, 2);

    EXPECT_STREQ(metric.properties.values[1].value.string_value, "Active");
    EXPECT_STREQ(metric.properties.values[2].value.string_value, "Deactive");

    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    testMetric->published();
    mockManager.setTime(1000);

    testMetric->setValue("Another String");

    mockManager.setTime(2000);
    testMetric->addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_STREQ(payload.metrics[1].name, "MetricName");
    EXPECT_STREQ(payload.metrics[1].value.string_value, "Another String");
    EXPECT_TRUE(payload.metrics[1].has_timestamp);
    EXPECT_EQ(payload.metrics[1].timestamp, 1000);
    EXPECT_FALSE(payload.metrics[1].has_properties);

    EXPECT_TRUE(testMetric->isDirty());

    testMetric->published();

    EXPECT_FALSE(testMetric->isDirty());

    TimeManager::reset();

    free_payload(&payload);
}