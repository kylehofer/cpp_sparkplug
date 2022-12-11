/*
 * File: BrokerTests.cpp
 * Project: sparkplug_c
 * Created Date: Sunday November 27th 2022
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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "PahoClient.h"
#include "Node.h"
#include "Device.h"
#include "Metrics/SimpleMetric.h"
#include "Metrics/CallbackMetric.h"
#include "utils/TestClientUtility.h"
#include "utils/PortForwarder.h"

#include <thread>
#include <chrono>

#define MAX_RETRIES 400
#define RETRY_TIMEOUT 5

const char CLIENT_ADDRESS[] = "tcp://localhost:1883";
const char DISCONNECT_ADDRESS[] = "tcp://localhost:1884";
const char CLIENT_CLIENT_ID[] = "unique_id";
const int BROKER_PORT = 1883;
const char BROKER_HOSTNAME[] = "localhost";

using ::testing::AtLeast;
using ::testing::Mock;
using ::testing::NotNull;
using ::testing::Return;

using namespace std;
using ::chrono::milliseconds;
using ::this_thread::sleep_for;

/**
 * @brief Test Suite for Broker tests.
 * Setup up a mosquitto broker for running tests
 */
class BrokerTests : public testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        if (brokerThread == nullptr)
        {
            brokerThread = new thread(&system, "mosquitto >/dev/null 2>&1");
            PortForwarder::portCheck(BROKER_PORT);
        }
    }

    static void TearDownTestSuite()
    {
        system("killall mosquitto");
        brokerThread->join();
        delete brokerThread;
        brokerThread = nullptr;
    }

    static std::thread *brokerThread;
};

std::thread *BrokerTests::brokerThread = nullptr;

/**
 * @brief Utility method for checking messages sequences
 * Handles memory allocation and cleanup
 *
 * @param message
 * @param sequence
 * @return true
 * @return false
 */
inline bool checkSequence(PayloadMessage message, uint64_t sequence)
{
    if (message.payloadlen == 0)
    {
        return false;
    }

    // Decode the payload
    org_eclipse_tahu_protobuf_Payload sparkplugPayload = org_eclipse_tahu_protobuf_Payload_init_zero;
    if (decode_payload(&sparkplugPayload, (uint8_t *)message.payload, message.payloadlen) < 0)
    {
        fprintf(stderr, "Failed to decode the payload\n");
        return false;
    }

    EXPECT_EQ(sparkplugPayload.seq, sequence);
    free_payload(&sparkplugPayload);
    return true;
}

/**
 * @brief Utility method for checking if data has been recieved by a test client.
 *
 * @param testClient
 * @param topic
 * @param sequence -1 if no sequence needs to be checked
 */
inline void expectData(TestClientUtility *testClient, const char *topic, int sequence)
{
    testClient->waitForData(MAX_RETRIES);

    bool hasData = testClient->hasData();

    ASSERT_TRUE(hasData);
    PayloadMessage receivedMessage;
    receivedMessage = testClient->pop();
    EXPECT_STREQ(receivedMessage.topic, topic);
    if (sequence >= 0)
    {
        EXPECT_TRUE(checkSequence(receivedMessage, sequence));
    }
    TestClientUtility::freePayload(receivedMessage);
}

// Wraps the expectData method with a SCOPED_TRACE
#define EXPECT_DATA(testClient, topic, sequence) \
    {                                            \
        SCOPED_TRACE("DATA_CHECK");              \
        expectData(testClient, topic, sequence); \
    }

// Tests a broker disconnection
TEST_F(BrokerTests, NodeWithDisconnect)
{
    PortForwarder sparkplugForwarder = PortForwarder(1884, BROKER_PORT);

    sparkplugForwarder.start();

    int retries = 0;

    TestClientUtility testClient;

    testClient.connect("localhost", BROKER_PORT);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5, NODE_CONTROL_NONE};

    ClientOptions clientOptions = {
        .address = "tcp://localhost:1884",
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 1,
        .keepAliveInterval = 1};

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);

    SimpleMetric testDeviceMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    testDevice.addMetric(&testDeviceMetric);

    SimpleMetric testNodeMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    node.addClient<PahoClient>(&clientOptions);
    node.addDevice(&testDevice);
    node.addMetric(&testNodeMetric);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    sparkplugForwarder.stop();

    retries = 0;
    while (node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    EXPECT_FALSE(node.isActive());

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NDEATH/NodeId", -1);

    sparkplugForwarder.start();

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);
    testClient.waitForData(MAX_RETRIES);

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NDATA/NodeId", 2);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DDATA/NodeId/DeviceName", 3);

    node.stop();
    sparkplugForwarder.stop();
}

// Tests Nodes with devices on the same publish time
TEST_F(BrokerTests, NodeWithDeviceSameTime)
{
    int retries = 0;

    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5, NODE_CONTROL_NONE};

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);

    SimpleMetric testDeviceMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    testDevice.addMetric(&testDeviceMetric);

    SimpleMetric testNodeMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    node.addClient<PahoClient>(&clientOptions);
    node.addDevice(&testDevice);
    node.addMetric(&testNodeMetric);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NDATA/NodeId", 2);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DDATA/NodeId/DeviceName", 3);

    node.stop();
}

// Tests a node with a Primary host
TEST_F(BrokerTests, NodeWithPrimaryHost)
{
    int retries = 0;

    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "MyPrimary", 5, NODE_CONTROL_NONE};

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
    };

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);

    SimpleMetric testDeviceMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    testDevice.addMetric(&testDeviceMetric);

    SimpleMetric testNodeMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    node.addClient<PahoClient>(&clientOptions);
    node.addDevice(&testDevice);
    node.addMetric(&testNodeMetric);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);

    testClient.publish("STATE/MyPrimary", (void *)"ONLINE", 7, true);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    testClient.publish("STATE/MyPrimary", (void *)"OFFLINE", 7, true);

    retries = 0;
    while (node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    testClient.publish("STATE/MyPrimary", (void *)"ONLINE", 7, true);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    node.stop();
}

// Tests rebirth commands
TEST_F(BrokerTests, NodeWithRebirthCommand)
{
    int retries = 0;

    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5, NODE_CONTROL_REBIRTH};

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
    };

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);

    SimpleMetric testDeviceMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    testDevice.addMetric(&testDeviceMetric);

    SimpleMetric testNodeMetric = SimpleMetric("MetricName", 20, METRIC_DATA_TYPE_INT32);

    node.addClient<PahoClient>(&clientOptions);
    node.addDevice(&testDevice);
    node.addMetric(&testNodeMetric);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    }

    ASSERT_TRUE(node.isActive());

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NDATA/NodeId", 2);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DDATA/NodeId/DeviceName", 3);

    org_eclipse_tahu_protobuf_Payload payload;
    get_next_payload(&payload);
    bool rebirth_value = true;

    add_simple_metric(&payload, "Node Control/Rebirth", true, 0, METRIC_DATA_TYPE_BOOLEAN, false, false, &rebirth_value, sizeof(rebirth_value));

    // Encoding a Rebirth message to test commands
    size_t buffer_length = 1024;
    uint8_t *binary_buffer = (uint8_t *)malloc(buffer_length * sizeof(uint8_t));
    size_t message_length = encode_payload(binary_buffer, buffer_length, &payload);
    testClient.publish("spBv1.0/GroupId/NCMD/NodeId", binary_buffer, message_length, false);

    // Memory
    free(binary_buffer);
    free_payload(&payload);

    testClient.waitForData(MAX_RETRIES);
    EXPECT_TRUE(testClient.hasData());
    testClient.pop();

    EXPECT_DATA(&testClient, "spBv1.0/GroupId/NBIRTH/NodeId", 0);
    EXPECT_DATA(&testClient, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName", 1);

    node.stop();
}
