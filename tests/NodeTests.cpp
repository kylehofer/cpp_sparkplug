/*
 * File: NodeTests.cpp
 * Project: cpp_sparkplug
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
#include "mocks/MockSparkplugClient.h"
#include "Node.h"

const char CLIENT_ADDRESS[] = "tcp://192.168.1.20:1883";
const char CLIENT_CLIENT_ID[] = "unique_id";

using ::testing::AtLeast;
using ::testing::Mock;
using ::testing::NotNull;
using ::testing::Return;

TEST(NodeTests, addClient)
{
    MockSparkplugClient *mockClient1, *mockClient2;

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "", 5, NODE_CONTROL_NONE};

    Node node = Node(&nodeOptions);

    ClientOptions clientOptions1 = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    ClientOptions clientOptions2 = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    ClientTopicOptions *topics;

    mockClient1 = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions1);
    mockClient2 = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions2);

    EXPECT_CALL(*mockClient1, configureClient(&clientOptions1)).WillOnce([mockClient1](ClientOptions *options)
                                                                         { return 0; });

    EXPECT_CALL(*mockClient2, configureClient(&clientOptions2)).WillOnce([mockClient2](ClientOptions *options)
                                                                         { return 0; });

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    topics = mockClient1->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic.c_str(), "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic.c_str(), "spBv1.0/GroupId/DCMD/NodeId/+");
    // EXPECT_EQ(topics->primaryHostTopic, nullptr);

    topics = mockClient2->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic.c_str(), "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic.c_str(), "spBv1.0/GroupId/DCMD/NodeId/+");
    // EXPECT_EQ(topics->primaryHostTopic, nullptr);
}

TEST(NodeTests, addPrimaryClients)
{
    MockSparkplugClient *mockClient1, *mockClient2;
    ClientTopicOptions *topics;

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5, NODE_CONTROL_NONE};

    Node node = Node(&nodeOptions);

    ClientOptions clientOptions1 = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    ClientOptions clientOptions2 = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    mockClient1 = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions1);
    mockClient2 = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions2);

    EXPECT_CALL(*mockClient1, configureClient(&clientOptions1)).WillOnce([mockClient1](ClientOptions *options)
                                                                         { return 0; });

    EXPECT_CALL(*mockClient2, configureClient(&clientOptions2)).WillOnce([mockClient2](ClientOptions *options)
                                                                         { return 0; });

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    topics = mockClient1->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic.c_str(), "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic.c_str(), "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_STREQ(topics->primaryHostTopic.c_str(), "STATE/PrimaryHost");

    topics = mockClient2->getTopics();

    EXPECT_STREQ(topics->nodeCommandTopic.c_str(), "spBv1.0/GroupId/NCMD/NodeId");
    EXPECT_STREQ(topics->deviceCommandTopic.c_str(), "spBv1.0/GroupId/DCMD/NodeId/+");
    EXPECT_STREQ(topics->primaryHostTopic.c_str(), "STATE/PrimaryHost");
}

TEST(NodeTests, enable)
{
    NodeOptions nodeNoGroupIdOptions = {
        "", "NodeId", "PrimaryHost", 5, NODE_CONTROL_NONE};

    NodeOptions nodeNoNodeIdOptions = {
        "GroupId", "", "PrimaryHost", 5, NODE_CONTROL_NONE};

    NodeOptions nodeNoHostOptions = {
        "GroupId", "NodeId", "", 5, NODE_CONTROL_NONE};

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5, NODE_CONTROL_NONE};

    Node nodeNoGroupId = Node(&nodeNoGroupIdOptions);
    Node nodeNoNodeId = Node(&nodeNoNodeIdOptions);
    Node nodeNoHost = Node(&nodeNoHostOptions);
    Node node = Node(&nodeOptions);
    Node nodeNoClient = Node(&nodeOptions);
    Node nodeFailClientConfig = Node(&nodeOptions);

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    MockSparkplugClient *mockClient1, *mockClient2, *mockClient3;

    nodeNoGroupId.addClient<MockSparkplugClient>(&clientOptions);
    nodeNoNodeId.addClient<MockSparkplugClient>(&clientOptions);
    mockClient1 = (MockSparkplugClient *)nodeNoHost.addClient<MockSparkplugClient>(&clientOptions);
    mockClient2 = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions);
    mockClient3 = (MockSparkplugClient *)nodeFailClientConfig.addClient<MockSparkplugClient>(&clientOptions);

    EXPECT_CALL(*mockClient1, configureClient(&clientOptions)).WillOnce([mockClient1](ClientOptions *options)
                                                                        { return 0; });

    EXPECT_CALL(*mockClient2, configureClient(&clientOptions)).WillOnce([mockClient2](ClientOptions *options)
                                                                        { return 0; });

    EXPECT_CALL(*mockClient3, configureClient(&clientOptions)).WillOnce([mockClient2](ClientOptions *options)
                                                                        { return -1; });

    EXPECT_EQ(nodeNoGroupId.enable(), ENABLE_INVALID_TOPICS);
    EXPECT_EQ(nodeNoNodeId.enable(), ENABLE_INVALID_TOPICS);
    EXPECT_EQ(nodeNoHost.enable(), ENABLE_SUCCESS);
    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);
    EXPECT_EQ(nodeNoClient.enable(), ENABLE_NO_CLIENTS);
    EXPECT_EQ(nodeFailClientConfig.enable(), ENABLE_CLIENT_CONFIG_FAIL);
}

TEST(NodeTests, executeHappyPrimaryTest)
{
    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "PrimaryHost", 5, NODE_CONTROL_NONE};

    Node node = Node(&nodeOptions);

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    MockSparkplugClient *mockClient;

    mockClient = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions);

    EXPECT_CALL(*mockClient, configureClient(&clientOptions)).WillOnce([mockClient](ClientOptions *options)
                                                                       { return 0; });

    EXPECT_CALL(*mockClient, clientConnect()).WillOnce([mockClient]()
                                                       { return 0; });

    EXPECT_NE(mockClient, nullptr);

    EXPECT_EQ(node.execute(0), -1);

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    EXPECT_EQ(node.execute(0), 1) << "No Clients are connected, we should expect the idle time.";

    EXPECT_CALL(*mockClient, subscribeToPrimaryHost()).WillOnce([mockClient]()
                                                                { return 0; });

    mockClient->connect();

    EXPECT_EQ(node.execute(0), 1) << "Client is connected, however it is not active yet.";

    mockClient->active();

    EXPECT_EQ(node.execute(0), 1) << "Client is connected, however the node is still waiting for primary host to activate the client.";

    EXPECT_CALL(*mockClient, subscribeToCommands()).WillOnce([mockClient]()
                                                             { return 0; });

    const char primaryHostTopic[] = "STATE/PrimaryHost";

    {
        MessageEventStruct messageEvent = {
            primaryHostTopic, (char *)"ONLINE", 7};

        node.onEvent(mockClient, CLIENT_MESSAGE, &messageEvent);
    }

    EXPECT_EQ(node.execute(0), 1) << "Client is connected, Primary Host message received, waiting for command subscription";

    PublishRequest *requestedPublish = nullptr;

    // We should expect a birth message
    EXPECT_CALL(*mockClient, request(NotNull())).WillOnce([&](PublishRequest *publishRequest)
                                                          {

        requestedPublish = publishRequest;
        return 0; });

    // Sending our activation back, aka command subscription success
    mockClient->active();

    ASSERT_EQ(requestedPublish, nullptr);

    node.sync();

    ASSERT_NE(requestedPublish, nullptr);
    EXPECT_EQ(requestedPublish->isBirth, true);
    EXPECT_EQ(requestedPublish->publisher, (Publishable *)&node);
    EXPECT_STREQ(requestedPublish->topic.c_str(), "spBv1.0/GroupId/NBIRTH/NodeId");

    // We should expect our brocket to be requested to send this request
    EXPECT_CALL(*mockClient, publishMessage(requestedPublish->topic, NotNull(), 24, &requestedPublish->token))
        .WillOnce([mockClient](std::string topic, uint8_t *buffer, size_t length, DeliveryToken *token)
                  { return 0; });

    mockClient->processRequest(requestedPublish);

    EXPECT_EQ(node.execute(0), 5) << "Client is connected, Primary Host received, Commands Subscribed.";

    EXPECT_EQ(node.execute(2), 5) << "A live publish should be active, timer should be held.";

    EXPECT_EQ(node.execute(5), 5) << "A live publish should be active, timer should be held.";

    node.onEvent(mockClient, CLIENT_DELIVERED, (Publishable *)&node);

    EXPECT_EQ(node.execute(2), 3) << "Publish has been acknowledged, time should decrement now";
    SparkplugClient::destroyRequest(requestedPublish);

    EXPECT_EQ(node.execute(5), 5) << "No new data to publish, so timer should lock at 5";
}

TEST(NodeTests, executeHappyTest)
{
    NodeOptions nodeOptions = {
        "GroupId", "NodeId", "", 5, NODE_CONTROL_NONE};

    Node node = Node(&nodeOptions);

    ClientOptions clientOptions = {
        .address = CLIENT_ADDRESS,
        .clientId = CLIENT_CLIENT_ID,
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5};

    MockSparkplugClient *mockClient;

    mockClient = (MockSparkplugClient *)node.addClient<MockSparkplugClient>(&clientOptions);

    EXPECT_CALL(*mockClient, configureClient(&clientOptions)).WillOnce([mockClient](ClientOptions *options)
                                                                       { return 0; });

    EXPECT_CALL(*mockClient, clientConnect()).WillOnce([mockClient]()
                                                       { return 0; });

    EXPECT_NE(mockClient, nullptr);

    EXPECT_EQ(node.execute(0), -1);

    EXPECT_EQ(node.enable(), ENABLE_SUCCESS);

    EXPECT_EQ(node.execute(0), 1) << "No Clients are connected, we should expect the idle time.";

    // Should not be called as it is not a primary host client
    EXPECT_CALL(*mockClient, subscribeToPrimaryHost()).Times(0);

    // A non Primary host client should subscribe instantly
    EXPECT_CALL(*mockClient, subscribeToCommands()).WillOnce([mockClient]()
                                                             { return 0; });

    mockClient->connect();

    EXPECT_EQ(node.execute(0), 1) << "Client is connected, and waiting for commands to subscribe.";

    const char primaryHostTopic[] = "STATE/PrimaryHost";

    // Sending message should have no affect
    {
        MessageEventStruct messageEvent = {
            primaryHostTopic, (char *)"ONLINE", 7};

        node.onEvent(mockClient, CLIENT_MESSAGE, &messageEvent);
    }

    EXPECT_EQ(node.execute(0), 1) << "Client is connected, Primary Host message received, waiting for command subscription";

    PublishRequest *requestedPublish = nullptr;

    // We should expect a birth message
    EXPECT_CALL(*mockClient, request(NotNull())).WillOnce([&](PublishRequest *publishRequest)
                                                          {
        requestedPublish = publishRequest;
        return 0; });

    // Sending our activation back, aka command subscription success
    mockClient->active();

    ASSERT_EQ(requestedPublish, nullptr);

    node.sync();

    ASSERT_NE(requestedPublish, nullptr);
    EXPECT_EQ(requestedPublish->isBirth, true);
    EXPECT_EQ(requestedPublish->publisher, (Publishable *)&node);
    EXPECT_STREQ(requestedPublish->topic.c_str(), "spBv1.0/GroupId/NBIRTH/NodeId");

    // We should expect our brocket to be requested to send this request
    EXPECT_CALL(*mockClient, publishMessage(requestedPublish->topic, NotNull(), 24, &requestedPublish->token))
        .WillOnce([mockClient](std::string topic, uint8_t *buffer, size_t length, DeliveryToken *token)
                  { return 0; });

    mockClient->processRequest(requestedPublish);

    EXPECT_EQ(node.execute(0), 5) << "Client is connected, Commands Subscribed.";

    EXPECT_EQ(node.execute(2), 5) << "A live publish should be active, timer should be held.";

    EXPECT_EQ(node.execute(5), 5) << "A live publish should be active, timer should be held.";

    node.onEvent(mockClient, CLIENT_DELIVERED, (Publishable *)&node);

    EXPECT_EQ(node.execute(2), 3) << "Publish has been acknowledged, time should decrement now";

    SparkplugClient::destroyRequest(requestedPublish);

    EXPECT_EQ(node.execute(5), 5) << "No new data to publish, so timer should lock at 5";
}
