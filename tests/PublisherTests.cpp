/*
 * File: PublisherTests.cpp
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

#include "Publisher.h"
#include "Metric.h"


TEST(Publisher, TestUpdate) {
    Publisher testPublisher = Publisher(30);

    EXPECT_EQ(testPublisher.update(5), 25);
    EXPECT_EQ(testPublisher.update(0), 25);
    EXPECT_EQ(testPublisher.update(10), 15);
    EXPECT_EQ(testPublisher.update(15), 30);
    EXPECT_EQ(testPublisher.update(5), 30);

    testPublisher.published();

    EXPECT_EQ(testPublisher.update(10), 20);
    EXPECT_EQ(testPublisher.update(19), 1);
    EXPECT_EQ(testPublisher.update(1000), 30);
}

TEST(Publisher, TestCanPublish) {
    Publisher testPublisher = Publisher(30);
    Metric testMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    Metric* metrics[] = { &testMetric };

    testPublisher.setMetrics(metrics, 1);

    EXPECT_EQ(testPublisher.update(30), 30);
    EXPECT_FALSE(testPublisher.canPublish());

    int newValue = 21;
    testMetric.setValue(&newValue);
    EXPECT_TRUE(testPublisher.canPublish());

    testPublisher.published();

    newValue = 22;
    testMetric.setValue(&newValue);
    EXPECT_FALSE(testPublisher.canPublish());

    EXPECT_EQ(testPublisher.update(30), 30);
    EXPECT_TRUE(testPublisher.canPublish());
}

TEST(Publisher, TestGetPayload) {
    Publisher testPublisher = Publisher(30);
    Metric testMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    Metric* metrics[] = { &testMetric };

    testPublisher.setMetrics(metrics, 1);

    EXPECT_EQ(testPublisher.update(30), 30);
    EXPECT_FALSE(testPublisher.canPublish());
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    
    int newValue = 21;
    testMetric.setValue(&newValue);
    EXPECT_TRUE(testPublisher.canPublish());

    org_eclipse_tahu_protobuf_Payload* payload;

    payload = testPublisher.getPayload();

    EXPECT_TRUE(payload != NULL);
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    EXPECT_FALSE(testPublisher.canPublish());

    free_payload(payload); 
    free(payload); 

    testPublisher.published();

    newValue = 22;
    testMetric.setValue(&newValue);
    EXPECT_FALSE(testPublisher.canPublish());

    EXPECT_EQ(testPublisher.update(30), 30);
    EXPECT_TRUE(testPublisher.canPublish());

    payload = testPublisher.getPayload();

    EXPECT_TRUE(payload != NULL);
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    EXPECT_FALSE(testPublisher.canPublish());

    free_payload(payload); 
    free(payload); 
}

TEST(Publisher, TestGetBirthPayload) {
    int newValue;
    Publisher testPublisher = Publisher(30);
    Metric testMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    Metric* metrics[] = { &testMetric };

    testPublisher.setMetrics(metrics, 1);

    EXPECT_EQ(testPublisher.update(5), 25);

    org_eclipse_tahu_protobuf_Payload* payload;

    EXPECT_FALSE(testPublisher.canPublish());
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    payload = testPublisher.getPayload(true);  
    EXPECT_TRUE(payload != NULL);
    
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    EXPECT_FALSE(testPublisher.canPublish());

    EXPECT_EQ(testPublisher.update(5), 30);
    EXPECT_EQ(testPublisher.update(100), 30);

    EXPECT_FALSE(testPublisher.canPublish());

    free_payload(payload); 
    free(payload); 

    payload = testPublisher.getPayload(true);  
    EXPECT_TRUE(payload != NULL);

    free_payload(payload); 
    free(payload); 

    testPublisher.published();

    newValue = 22;
    testMetric.setValue(&newValue);
    EXPECT_FALSE(testPublisher.canPublish());

    EXPECT_EQ(testPublisher.update(30), 30);
    EXPECT_TRUE(testPublisher.canPublish());

    payload = testPublisher.getPayload();

    EXPECT_TRUE(payload != NULL);
    EXPECT_TRUE(testPublisher.getPayload() == NULL);
    EXPECT_FALSE(testPublisher.canPublish());

    free_payload(payload); 
    free(payload); 
}