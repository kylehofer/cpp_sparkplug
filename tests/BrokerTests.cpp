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
#include "TestClientUtility.h"

#include <thread>

#define MAX_RETRIES 400
#define RETRY_TIMEOUT 5

const char CLIENT_ADDRESS[] = "tcp://localhost:1883";
const char CLIENT_CLIENT_ID[] = "unique_id";

using ::testing::Mock;
using ::testing::NotNull;
using ::testing::AtLeast;
using ::testing::Return;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

bool portCheck()
{
    int portno     = 1883;
    char *hostname = "localhost";
 
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
    server = gethostbyname(hostname);
 
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
    }
 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
 
    serv_addr.sin_port = htons(portno);

    while (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {};
 
    close(sockfd);
}

TEST(BrokerTests, NodeWithDeviceSameTime) {
    std::thread brokerThread(&system, "mosquitto >/dev/null 2>&1");

    portCheck();
    int retries = 0;

    PayloadMessage recievedMessage;
    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);
    Device* devices[] = { &testDevice };

    Metric testDeviceMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* deviceMetrics[] = { &testDeviceMetric };

    testDevice.setMetrics(deviceMetrics, 1);

    Metric testNodeMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* nodeMetrics[] = { &testNodeMetric };

    node.addClient<PahoClient>(&clientOptions);
    node.setDevices(devices, 1);
    node.setMetrics(nodeMetrics, 1);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);
    
    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NBIRTH/NodeId");
    TestClientUtility::freePayload(recievedMessage);

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName");
    TestClientUtility::freePayload(recievedMessage);

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(executeTime));
    

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NDATA/NodeId");
    TestClientUtility::freePayload(recievedMessage);

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DDATA/NodeId/DeviceName");
    TestClientUtility::freePayload(recievedMessage);

    node.stop();
    system("killall mosquitto");
    brokerThread.join();
}

TEST(BrokerTests, NodeWithDeviceDisconnect) {
    std::thread brokerThread(&system, "mosquitto >/dev/null 2>&1");

    portCheck();
    
    int retries = 0;

    PayloadMessage recievedMessage;
    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);
    Device* devices[] = { &testDevice };

    Metric testDeviceMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* deviceMetrics[] = { &testDeviceMetric };

    testDevice.setMetrics(deviceMetrics, 1);

    Metric testNodeMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* nodeMetrics[] = { &testNodeMetric };

    node.addClient<PahoClient>(&clientOptions);
    node.setDevices(devices, 1);
    node.setMetrics(nodeMetrics, 1);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);
    
    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NBIRTH/NodeId");

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName");

    system("killall mosquitto");

    retries = 0;
    while (node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    std::thread newBrokerThread(&system, "mosquitto >/dev/null 2>&1");

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.connect("localhost", 1883);

    testClient.waitForReady(MAX_RETRIES);

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(executeTime));

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NDATA/NodeId");

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DDATA/NodeId/DeviceName");

    node.stop();
    system("killall mosquitto");
    
    brokerThread.join();
    newBrokerThread.join();
}


TEST(BrokerTests, NodeWithPrimaryHost) {
    std::thread brokerThread(&system, "mosquitto >/dev/null 2>&1");

    portCheck();
    
    int retries = 0;

    PayloadMessage recievedMessage;
    TestClientUtility testClient;

    testClient.connect("localhost", 1883);

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "MyPrimary", 5
    };

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    Node node = Node(&nodeOptions);

    Device testDevice = Device("DeviceName", 5);
    Device* devices[] = { &testDevice };

    Metric testDeviceMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* deviceMetrics[] = { &testDeviceMetric };

    testDevice.setMetrics(deviceMetrics, 1);

    Metric testNodeMetric = Metric("MetricName", 20, METRIC_DATA_TYPE_INT32);
    Metric* nodeMetrics[] = { &testNodeMetric };

    node.addClient<PahoClient>(&clientOptions);
    node.setDevices(devices, 1);
    node.setMetrics(nodeMetrics, 1);
    node.enable();

    testClient.waitForReady(MAX_RETRIES);

    const char payload[] = "ONLINE";

    testClient.publish("STATE/MyPrimary", (void *)"ONLINE", 7, true);
    
    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NBIRTH/NodeId");

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DBIRTH/NodeId/DeviceName");


    testClient.publish("STATE/MyPrimary", (void *)"OFFLINE", 7, true);

    retries = 0;
    while (node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.publish("STATE/MyPrimary", (void *)"ONLINE", 7, true);

    retries = 0;
    while (!node.isActive() && retries++ < MAX_RETRIES)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
    }

    testClient.connect("localhost", 1883);

    testClient.waitForReady(MAX_RETRIES);

    int32_t executeTime = 5;
    int32_t value = 5;

    testNodeMetric.setValue(&value);
    testDeviceMetric.setValue(&value);

    executeTime = node.execute(executeTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(executeTime));

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/NDATA/NodeId");

    testClient.waitForData(MAX_RETRIES);

    EXPECT_TRUE(testClient.hasData());
    recievedMessage = testClient.pop();
    EXPECT_STREQ(recievedMessage.topic, "spBv1.0/GroupId/DDATA/NodeId/DeviceName");

    node.stop();
    system("killall mosquitto");
    
    brokerThread.join();
}
