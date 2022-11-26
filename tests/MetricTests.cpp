#include "gtest/gtest.h"

#include <tahu.h>

#include "Metric.h"

typedef struct {
    int property1;
    int property2;
} TestStruct;

// template Metric::Metric(const char *name, int data, uint8_t dataType);
// template Metric::Metric(const char *name, TestStruct data, uint8_t dataType);

TEST(Metric, TestSimpleDirty) {
    Metric testMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    EXPECT_FALSE(testMetric.isDirty());

    int newValue;

    newValue = 20;

    testMetric.setValue(&newValue);
    EXPECT_FALSE(testMetric.isDirty());
    EXPECT_EQ(*(int*)testMetric.getData(), newValue);
    EXPECT_NE(testMetric.getData(), &newValue);

    newValue = 21;

    testMetric.setValue(&newValue);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(*(int*)testMetric.getData(), newValue);
    EXPECT_NE(testMetric.getData(), &newValue);

    newValue = 21;

    testMetric.setValue(&newValue);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(*(int*)testMetric.getData(), newValue);
    EXPECT_NE(testMetric.getData(), &newValue);

    newValue = 20;

    testMetric.setValue(&newValue);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(*(int*)testMetric.getData(), newValue);
    EXPECT_NE(testMetric.getData(), &newValue);
}

TEST(Metric, TestStringDirty) {
    static const int BUFFER_SIZE = 20;
    
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];

    memset(buffer1, 0, BUFFER_SIZE);
    memset(buffer2, 0, BUFFER_SIZE);

    memcpy(buffer1, "SomeString", 11);
    
    Metric testMetric = Metric("MetricName", buffer1, BUFFER_SIZE, METRIC_DATA_TYPE_STRING);

    EXPECT_FALSE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), buffer1, BUFFER_SIZE), 0);
    EXPECT_NE(testMetric.getData(), buffer1);

    memcpy(buffer2, "SomeString", 11);

    testMetric.setValue(buffer2);
    EXPECT_FALSE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), buffer2, BUFFER_SIZE), 0);
    EXPECT_NE(testMetric.getData(), buffer2);

    memcpy(buffer2, "AnotherString", 11);

    testMetric.setValue(buffer2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), buffer2, BUFFER_SIZE), 0);
    EXPECT_NE(testMetric.getData(), buffer2);

    memcpy(buffer2, "AnotherString", 11);

    testMetric.setValue(buffer2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), buffer2, BUFFER_SIZE), 0);
    EXPECT_NE(testMetric.getData(), buffer2);

    memcpy(buffer2, "SomeString", 11);

    testMetric.setValue(buffer2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), buffer2, BUFFER_SIZE), 0);
    EXPECT_NE(testMetric.getData(), buffer2);
}

TEST(Metric, TestStructDirty) {
    TestStruct value1 = { 5, 10 };
    TestStruct value2 = { 5, 10 };

    static const int STRUCT_SIZE = sizeof(TestStruct);

    Metric testMetric = Metric("MetricName", value1, METRIC_DATA_TYPE_INT32);

    EXPECT_FALSE(testMetric.isDirty());

    testMetric.setValue(&value2);
    EXPECT_FALSE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value1, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value1);

    value2 = { 10, 5 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);

    value2 = { 10, 5 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);

    value2 = { 5, 10 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);
}

TEST(Metric, TestStructPointerDirty) {
    TestStruct value1 = { 5, 10 };
    TestStruct value2 = { 5, 10 };

    static const int STRUCT_SIZE = sizeof(TestStruct);

    Metric testMetric = Metric("MetricName", &value1, STRUCT_SIZE, METRIC_DATA_TYPE_INT32);

    EXPECT_FALSE(testMetric.isDirty());

    testMetric.setValue(&value2);
    EXPECT_FALSE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value1, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value1);

    value2 = { 10, 5 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);

    value2 = { 10, 5 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);

    value2 = { 5, 10 };

    testMetric.setValue(&value2);
    EXPECT_TRUE(testMetric.isDirty());
    EXPECT_EQ(memcmp(testMetric.getData(), &value2, STRUCT_SIZE), 0);
    EXPECT_NE((TestStruct*) testMetric.getData(), &value2);
}

TEST(Metric, TestAddToPayload) {
    char metricName[] = {"MetricName"};
    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);

    Metric testMetric = Metric(metricName, 20, METRIC_DATA_TYPE_INT32);

    testMetric.addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 0);

    testMetric.addToPayload(&payload, true);
    EXPECT_EQ(payload.metrics_count, 1);
    EXPECT_EQ(payload.metrics[0].value.int_value, 20);

    testMetric.addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 1);

    int newValue = 50;

    testMetric.setValue(&newValue);
    testMetric.addToPayload(&payload);
    EXPECT_EQ(payload.metrics_count, 2);
    EXPECT_EQ(payload.metrics[1].value.int_value, 50);

    EXPECT_TRUE(testMetric.isDirty());

    testMetric.published();

    EXPECT_FALSE(testMetric.isDirty());

    free_payload(&payload); 
}