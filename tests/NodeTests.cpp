/*
 * File: NodeTests.cpp
 * Project: sparkplug_c
 * Created Date: Saturday November 26th 2022
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
#include "MockSparkplugBroker.h"
#include "Node.h"


const char BROKER_ADDRESS[] = "tcp://192.168.1.20:1883";
const char BROKER_CLIENT_ID[] = "unique_id";

using ::testing::Mock;
using ::testing::NotNull;
using ::testing::AtLeast;
using ::testing::Return;

TEST(NodeTests, addBroker) {
    MockSparkplugBroker *mockBrocker1, *mockBrocker2;

    
    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    Node node = Node(&nodeOptions);


    BrokerOptions brokerOptions1 = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    BrokerOptions brokerOptions2 = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    BrokerTopicOptions *topics;

    mockBrocker1 = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions1);
    mockBrocker2 = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions2);


    EXPECT_CALL(*mockBrocker1, configureClient(&brokerOptions1)).WillOnce([mockBrocker1](BrokerOptions *options) {
        return 0;
    });


    EXPECT_CALL(*mockBrocker2, configureClient(&brokerOptions2)).WillOnce([mockBrocker2](BrokerOptions *options) {
        return 0;
    });

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    topics = mockBrocker1->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic, "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic, "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_EQ(topics->primaryHostTopic, nullptr);

    topics = mockBrocker2->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic, "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic, "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_EQ(topics->primaryHostTopic, nullptr);
}

TEST(NodeTests, addPrimaryBrokers) {
    MockSparkplugBroker *mockBrocker1, *mockBrocker2;
    BrokerTopicOptions* topics;

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5
    };

    Node node = Node(&nodeOptions);

    BrokerOptions brokerOptions1 = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    BrokerOptions brokerOptions2 = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };


    mockBrocker1 = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions1);
    mockBrocker2 = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions2);


    EXPECT_CALL(*mockBrocker1, configureClient(&brokerOptions1)).WillOnce([mockBrocker1](BrokerOptions *options) {
        return 0;
    });


    EXPECT_CALL(*mockBrocker2, configureClient(&brokerOptions2)).WillOnce([mockBrocker2](BrokerOptions *options) {
        return 0;
    });

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    topics = mockBrocker1->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic, "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic, "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_STREQ(topics->primaryHostTopic, "STATE/PrimaryHost");

    topics = mockBrocker2->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic, "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic, "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_STREQ(topics->primaryHostTopic, "STATE/PrimaryHost");
}

TEST(NodeTests, enable) {
    NodeOptions nodeNoGroupIdOptions = {
        NULL, "NodeId", "PrimaryHost", 5
    };

    NodeOptions nodeNoNodeIdOptions = {
        "GroupId", NULL, "PrimaryHost", 5
    };

    NodeOptions nodeNoHostOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5
    };

    Node nodeNoGroupId = Node(&nodeNoGroupIdOptions);
    Node nodeNoNodeId = Node(&nodeNoNodeIdOptions);
    Node nodeNoHost = Node(&nodeNoHostOptions);
    Node node = Node(&nodeOptions);
    Node nodeNoBroker = Node(&nodeOptions);
    Node nodeFailClientConfig = Node(&nodeOptions);

    BrokerOptions brokerOptions = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    MockSparkplugBroker *mockBrocker1, *mockBrocker2, *mockBrocker3;

    nodeNoGroupId.addBroker<MockSparkplugBroker>(&brokerOptions);
    nodeNoNodeId.addBroker<MockSparkplugBroker>(&brokerOptions);
    mockBrocker1 = (MockSparkplugBroker*) nodeNoHost.addBroker<MockSparkplugBroker>(&brokerOptions);
    mockBrocker2 = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions);
    mockBrocker3 = (MockSparkplugBroker*) nodeFailClientConfig.addBroker<MockSparkplugBroker>(&brokerOptions);
    // node.addBroker<MockSparkplugBroker>(&brokerOptions);

    EXPECT_CALL(*mockBrocker1, configureClient(&brokerOptions)).WillOnce([mockBrocker1](BrokerOptions *options) {
        return 0;
    });


    EXPECT_CALL(*mockBrocker2, configureClient(&brokerOptions)).WillOnce([mockBrocker2](BrokerOptions *options) {
        return 0;
    });

    EXPECT_CALL(*mockBrocker3, configureClient(&brokerOptions)).WillOnce([mockBrocker2](BrokerOptions *options) {
        return -1;
    });

    EXPECT_EQ(nodeNoGroupId.enable(), ENABLE_INVALID_TOPICS);
    EXPECT_EQ(nodeNoNodeId.enable(), ENABLE_INVALID_TOPICS);
    EXPECT_EQ(nodeNoHost.enable(), ENABLE_SUCCESS);
    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);
    EXPECT_EQ(nodeNoBroker.enable(), ENABLE_NO_BROKERS);
    EXPECT_EQ(nodeFailClientConfig.enable(), ENABLE_BROKER_CONFIG_FAIL);

}

TEST(NodeTests, executeHappyPrimaryTest) {
    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5
    };

    Node node = Node(&nodeOptions);

    BrokerOptions brokerOptions = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    MockSparkplugBroker *mockBrocker;

    mockBrocker = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions);

    EXPECT_CALL(*mockBrocker, configureClient(&brokerOptions)).WillOnce([mockBrocker](BrokerOptions *options) {
        return 0;
    });

    EXPECT_CALL(*mockBrocker, clientConnect()).WillOnce([mockBrocker]() {
        return 0;
    });

    EXPECT_NE(mockBrocker, nullptr);

    EXPECT_EQ(node.execute(0), -1);

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    EXPECT_EQ(node.execute(0), 1) << "No Brokers are connected, we should expect the idle time.";

    EXPECT_CALL(*mockBrocker, subscribeToPrimaryHost()).WillOnce([mockBrocker]() {
        return 0;
    });

    mockBrocker->connect();

    EXPECT_EQ(node.execute(0), 1) << "Broker is connected, however it is not active yet.";

    mockBrocker->active();

    EXPECT_EQ(node.execute(0), 1) << "Broker is connected, however the node is still waiting for primary host to activate the broker.";

    SparkplugMessage message = {
        .payloadlen = 7,
        .payload = (char*) "ONLINE"
    };

    EXPECT_CALL(*mockBrocker, subscribeToCommands()).WillOnce([mockBrocker]() {
        return 0;
    });

    const char primaryHostTopic[] = "STATE/PrimaryHost";

    node.onMessage(mockBrocker, primaryHostTopic, 18, &message);

    EXPECT_EQ(node.execute(0), 1) << "Broker is connected, Primary Host message recieved, waiting for command subscription";

    PublishRequest* requestedPublish;

    // We should expect a birth message
    EXPECT_CALL(*mockBrocker, requestPublish(NotNull())).WillOnce([&](PublishRequest* publishRequest) {
        requestedPublish = publishRequest;
        return 0;
    });

    // Sending our activation back, aka command subscription success
    mockBrocker->active();

    EXPECT_NE(requestedPublish, nullptr);
    EXPECT_EQ(requestedPublish->isBirth, true);
    EXPECT_EQ(requestedPublish->publisher, (Publisher*) &node);
    EXPECT_STREQ(requestedPublish->topic, "spBv1.0/GroupId/NBIRTH/NodeId");

    // We should expect our brocket to be requested to send this request
    EXPECT_CALL(*mockBrocker, publishMessage(requestedPublish->topic, NotNull(), 9, &requestedPublish->token))
    .WillOnce([mockBrocker](const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token) {
        return 0;
    });

    mockBrocker->publish(requestedPublish);

    EXPECT_EQ(node.execute(0), 5) << "Broker is connected, Primary Host Recieved, Commands Subscribed.";

    EXPECT_EQ(node.execute(2), 5) << "A live publish should be active, timer should be held.";

    EXPECT_EQ(node.execute(5), 5) << "A live publish should be active, timer should be held.";

    node.onDelivery(mockBrocker, requestedPublish);

    EXPECT_EQ(node.execute(2), 3) << "Publish has been acknowledged, time should decrement now";

    SparkplugBroker::destroyRequest(requestedPublish);

    EXPECT_EQ(node.execute(5), 5) << "No new data to publish, so timer should lock at 5";
}

TEST(NodeTests, executeHappyTest) {
    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    Node node = Node(&nodeOptions);

    BrokerOptions brokerOptions = {
        .address = BROKER_ADDRESS,
        .clientId = BROKER_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    MockSparkplugBroker *mockBrocker;

    mockBrocker = (MockSparkplugBroker*) node.addBroker<MockSparkplugBroker>(&brokerOptions);

    EXPECT_CALL(*mockBrocker, configureClient(&brokerOptions)).WillOnce([mockBrocker](BrokerOptions *options) {
        return 0;
    });

    EXPECT_CALL(*mockBrocker, clientConnect()).WillOnce([mockBrocker]() {
        return 0;
    });

    EXPECT_NE(mockBrocker, nullptr);

    EXPECT_EQ(node.execute(0), -1);

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    EXPECT_EQ(node.execute(0), 1) << "No Brokers are connected, we should expect the idle time.";

    // Should not be called as it is not a primary host broker
    EXPECT_CALL(*mockBrocker, subscribeToPrimaryHost()).Times(0);

    // A non Primary host broker should subscribe instantly
    EXPECT_CALL(*mockBrocker, subscribeToCommands()).WillOnce([mockBrocker]() {
        return 0;
    });

    mockBrocker->connect();

    EXPECT_EQ(node.execute(0), 1) << "Broker is connected, and waiting for commands to subscribe.";

    SparkplugMessage message = {
        .payloadlen = 7,
        .payload = (char*) "ONLINE"
    };

    const char primaryHostTopic[] = "STATE/PrimaryHost";

    // Sending message should have no affect
    node.onMessage(mockBrocker, primaryHostTopic, 18, &message);

    EXPECT_EQ(node.execute(0), 1) << "Broker is connected, Primary Host message recieved, waiting for command subscription";

    PublishRequest* requestedPublish;

    // We should expect a birth message
    EXPECT_CALL(*mockBrocker, requestPublish(NotNull())).WillOnce([&](PublishRequest* publishRequest) {
        requestedPublish = publishRequest;
        return 0;
    });

    // Sending our activation back, aka command subscription success
    mockBrocker->active();

    EXPECT_NE(requestedPublish, nullptr);
    EXPECT_EQ(requestedPublish->isBirth, true);
    EXPECT_EQ(requestedPublish->publisher, (Publisher*) &node);
    EXPECT_STREQ(requestedPublish->topic, "spBv1.0/GroupId/NBIRTH/NodeId");

    // We should expect our brocket to be requested to send this request
    EXPECT_CALL(*mockBrocker, publishMessage(requestedPublish->topic, NotNull(), 9, &requestedPublish->token))
    .WillOnce([mockBrocker](const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token) {
        return 0;
    });

    mockBrocker->publish(requestedPublish);

    EXPECT_EQ(node.execute(0), 5) << "Broker is connected, Commands Subscribed.";

    EXPECT_EQ(node.execute(2), 5) << "A live publish should be active, timer should be held.";

    EXPECT_EQ(node.execute(5), 5) << "A live publish should be active, timer should be held.";

    node.onDelivery(mockBrocker, requestedPublish);

    EXPECT_EQ(node.execute(2), 3) << "Publish has been acknowledged, time should decrement now";

    SparkplugBroker::destroyRequest(requestedPublish);

    EXPECT_EQ(node.execute(5), 5) << "No new data to publish, so timer should lock at 5";
}