/*
 * File: Node.cpp
 * Project: cpp_sparkplug
 * Created Date: Sunday September 25th 2022
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

#include "Node.h"
#include <tahu.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>

#define NDATA "NDATA"
#define NBIRTH "NBIRTH"
#define NCMD "NCMD"
#define NDEATH "NDEATH"

#define DDATA "DDATA"
#define DBIRTH "DBIRTH"
#define DCMD "DCMD"
#define DDEATH "DDEATH"

#define PRIMARY_HOST_TOPIC_BUILDER "STATE/%s"
#define NODE_TOPIC_BUILDER "%s/%s/%s/%s"
#define DEVICE_TOPIC_BUILDER "%s/%s/%s/%s/%s"

#define EXECUTE_IDLE_DELAY 1

using namespace std;

#define SPARKPLUG_NAMESPACE "spBv1.0"

/**
 * @brief Helper function to add node controls to a payload
 *
 * @param payload
 * @return org_eclipse_tahu_protobuf_Payload*
 */
static org_eclipse_tahu_protobuf_Payload *addNodeControl(org_eclipse_tahu_protobuf_Payload *payload)
{
    bool next_server_value = false;
    add_simple_metric(payload, "Node Control/Next Server", false, 0, METRIC_DATA_TYPE_BOOLEAN, false, false, &next_server_value, sizeof(next_server_value));
    bool rebirth_value = false;
    add_simple_metric(payload, "Node Control/Rebirth", false, 0, METRIC_DATA_TYPE_BOOLEAN, false, false, &rebirth_value, sizeof(rebirth_value));
    bool reboot_value = false;
    add_simple_metric(payload, "Node Control/Reboot", false, 0, METRIC_DATA_TYPE_BOOLEAN, false, false, &reboot_value, sizeof(reboot_value));
    return payload;
}

Node::Node() : Node(NULL) {}
Node::Node(NodeOptions *options) : Publisher(options != NULL ? options->publishPeriod : 30), topicsConfigured(false), enabled(false), deviceCount(0), activeClient(NULL), asyncLock(new mutex())
{
    if (options != NULL)
    {
        configureTopics(options->groupId, options->nodeId, options->primaryHost);
    }
}

Node::~Node()
{
    for (auto client : clients)
    {
        delete client;
    }
    delete asyncLock;
    if (topicsConfigured)
    {
        free(nodeBaseTopic);
        free(deviceBaseTopic);
        if (clientTopics.nodeCommandTopic != NULL)
        {
            free(clientTopics.nodeCommandTopic);
        }
        if (clientTopics.deviceCommandTopic != NULL)
        {
            free(clientTopics.deviceCommandTopic);
        }
        if (clientTopics.primaryHostTopic != NULL)
        {
            free(clientTopics.primaryHostTopic);
        }
    }
}

void Node::connect()
{
    lock_guard<mutex> lock(*asyncLock);
    for (auto client : clients)
    {
        client->connect();
    }
}

int Node::enable()
{
    if (!topicsConfigured)
    {
        cout << "Cannot enable node as it has invalid topics.\n";
        return ENABLE_INVALID_TOPICS;
    }

    if (clients.size() == 0)
    {
        cout << "Cannot enable node as it has no Clients added.\n";
        return ENABLE_NO_CLIENTS;
    }

    for (auto client : clients)
    {
        if (client->configure(&clientTopics) != 0)
        {
            return ENABLE_CLIENT_CONFIG_FAIL;
        };
    }

    enabled = true;

    return ENABLE_SUCCESS;
}

void Node::publishBirth()
{
    reset_sparkplug_sequence();
    publish(true);
    for (int8_t i = (deviceCount - 1); i >= 0; i--)
    {
        publish(devices[i], true);
    }
}

void Node::configureTopics(const char *groupId, const char *nodeId, const char *primaryHost)
{
    if (groupId != NULL && nodeId != NULL)
    {
        size_t nameSpaceSize, groupIdSize, nodeIdSize, primaryHostSize;
        size_t miscSize;

        nameSpaceSize = strlen(SPARKPLUG_NAMESPACE);
        groupIdSize = strlen(groupId);
        nodeIdSize = strlen(nodeId);

        // 3 slashes, 2 for printf sub, 1 for null character
        miscSize = 3 + 2 + 1;
        nodeBaseTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);

        // 4 slashes, 4 for two printf subs, 1 for null character
        miscSize = 4 + 4 + 1;
        deviceBaseTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);

        // Build our base topics for publishing and subscribing
        sprintf(nodeBaseTopic, NODE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, "\%s", nodeId);
        sprintf(deviceBaseTopic, DEVICE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, "\%s", nodeId, "\%s");

        char *nodeCommandTopic, *deviceCommandTopic, *primaryHostTopic;

        // 3 slashes, 4 for NCMD, 1 for null character
        miscSize = 3 + 4 + 1;
        nodeCommandTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);
        sprintf(nodeCommandTopic, NODE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, NCMD, nodeId);

        // 4 slashes, 4 for NCMD, 1 for +, 1 for null character
        miscSize = 4 + 4 + 1 + 1;
        deviceCommandTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);
        sprintf(deviceCommandTopic, DEVICE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, DCMD, nodeId, "+");

        if (primaryHost != NULL)
        {
            primaryHostSize = strlen(primaryHost);
            // 5 for STATE, 1 for slash, 1 for null character
            miscSize = 5 + 1 + 1;
            primaryHostTopic = (char *)malloc(primaryHostSize + miscSize);
            sprintf(primaryHostTopic, PRIMARY_HOST_TOPIC_BUILDER, primaryHost);

            setClientMode(PRIMARY_HOST);
        }
        else
        {
            primaryHostTopic = NULL;
            setClientMode(SINGLE);
        }

        clientTopics = {
            nodeCommandTopic,
            deviceCommandTopic,
            primaryHostTopic};

        topicsConfigured = true;
    }
}

int Node::publish(bool isBirth = false)
{
    if (!this->canPublish() && !isBirth)
    {
        return 0;
    }

    char *topic;
    {
        char nodeMessageTopic[MAX_TOPIC_LENGTH];
        sprintf(nodeMessageTopic, nodeBaseTopic, isBirth ? NBIRTH : NDATA);

        size_t topicLength = strlen(nodeMessageTopic) + 1;

        topic = (char *)malloc(topicLength);

        strcpy(topic, nodeMessageTopic);
    }

    PublishRequest *publishRequest = (PublishRequest *)malloc(sizeof(PublishRequest));
    publishRequest->isBirth = isBirth;
    publishRequest->publisher = (Publisher *)this;
    publishRequest->topic = topic;

    return clients.front()->requestPublish(publishRequest);
}

int Node::publish(Device *device, bool isBirth = false)
{
    if (!device->canPublish() && !isBirth)
    {
        return 0;
    }

    char *topic;
    {
        char deviceMessageTopic[MAX_TOPIC_LENGTH];
        sprintf(deviceMessageTopic, deviceBaseTopic, isBirth ? DBIRTH : DDATA, device->getName());

        size_t topicLength = strlen(deviceMessageTopic) + 1;

        topic = (char *)malloc(topicLength);

        strcpy(topic, deviceMessageTopic);
    }

    PublishRequest *publishRequest = (PublishRequest *)malloc(sizeof(PublishRequest));
    publishRequest->isBirth = isBirth;
    publishRequest->publisher = (Publisher *)device;
    publishRequest->topic = topic;

    return clients.front()->requestPublish(publishRequest);
}

void Node::setClientMode(SparkplugClientMode mode)
{
    lock_guard<mutex> lock(*asyncLock);
    this->hostMode = mode;
}

SparkplugClientMode Node::getClientMode()
{
    lock_guard<mutex> lock(*asyncLock);
    return hostMode;
}

void Node::setActiveClient(SparkplugClient *activeClient)
{
    lock_guard<mutex> lock(*asyncLock);

    if (activeClient == this->activeClient)
    {
        return;
    }

    if (this->activeClient != NULL)
    {
        this->activeClient->deactivate();
    }

    this->activeClient = activeClient;

    if (activeClient != NULL)
    {
        publishBirth();
    }
}

void Node::activateClient(SparkplugClient* client)
{
    lock_guard<mutex> lock(*asyncLock);

    if (client == this->activeClient)
    {
        return;
    }

    if (client != NULL)
    {
        client->activate();
    }
}

void Node::deactivateClient(SparkplugClient* client)
{
    bool isActiveClient;
    asyncLock->lock();
    isActiveClient = client != this->activeClient;
    asyncLock->unlock();

    if (isActiveClient)
    {
        return;
    }

    setActiveClient(NULL);
}

SparkplugClient *Node::getActiveClient()
{
    lock_guard<mutex> lock(*asyncLock);
    return activeClient;
}

int32_t Node::execute(int32_t executeTime)
{
    if (!enabled)
    {
        cout << "Cannot execute as the node has not been enabled\n";
        return -1;
    }
    if (!isActive())
    {
        return EXECUTE_IDLE_DELAY;
    }
    SparkplugClient *activeClient = getActiveClient();
    int32_t nextExecute = 0xFFFF;
    nextExecute = min(update(executeTime), nextExecute);
    if (canPublish())
    {
        publish();
    }

    for (int8_t i = (deviceCount - 1); i >= 0; i--)
    {
        Device *device = devices[i];
        nextExecute = min(device->update(executeTime), nextExecute);

        if (device->canPublish())
        {
            publish(device);
        }
    }

    return nextExecute;
}

void Node::begin()
{
    if (!enabled)
    {
        cout << "Cannot execute as the node has not been enabled\n";
        return;
    }

    running = true;

    int32_t nextExecute = 0;

    for (;;)
    {
        if (!running)
        {
            // Potential check for getting out of an endless loop
            // Idea is to run this on its thread
            return;
        }

        nextExecute = execute(nextExecute);
        std::this_thread::sleep_for(std::chrono::seconds(nextExecute));
    }
}

SparkplugClient* Node::addClient(SparkplugClient *client)
{
    clients.push_back(client);
    return client;
}

int Node::nextServer()
{
    return 0;
}

void Node::setDevices(Device **things, int8_t count)
{
    this->devices = things;
    this->deviceCount = count;
}

void Node::onDelivery(SparkplugClient *client, PublishRequest *request)
{
    request->publisher->published();
    // TODO: Delivery Handler
}

int Node::onMessage(SparkplugClient *client, const char *topicName, int topicLen, SparkplugMessage *message)
{
    if (client == getActiveClient())
    {
        if (strstr(topicName, NCMD) != NULL)
        {
            cout << "Node Command\n";
        }
        else if (strstr(topicName, DCMD) != NULL)
        {
            cout << "Device Command\n";
        }
    }

    if (clientTopics.primaryHostTopic != NULL && strcmp(topicName, clientTopics.primaryHostTopic) == 0)
    {
        if (strncmp((char *)message->payload, "ONLINE", message->payloadlen) == 0)
        {
            // Primary Host Online
            if (getActiveClient() != client)
            {
                activateClient(client);
            }
        }
        else if (strncmp((char *)message->payload, "OFFLINE", message->payloadlen) == 0)
        {
            if (getActiveClient() == client)
            {
                deactivateClient(client);
            }
        }
    }
    return 0;
}

void Node::onActive(SparkplugClient *client)
{
    setActiveClient(client);
}

void Node::onDeactive(SparkplugClient *client)
{
}

void Node::stop()
{
    for (auto client : clients)
    {
        client->deactivate();
        client->disconnect();
    }
}

void Node::onConnect(SparkplugClient *client)
{
    if (getClientMode() == SINGLE)
    {
        activateClient(client);
    }
}

void Node::onDisconnect(SparkplugClient *client, char *cause)
{
    switch (getClientMode())
    {
    case SINGLE:
    case PRIMARY_HOST:
        break;
    default:
        break;
    }

    // TODO: Disconnect Handler
}

org_eclipse_tahu_protobuf_Payload *Node::initializePayload(bool isBirth)
{
    return isBirth ? addNodeControl(Publisher::initializePayload(isBirth)) : Publisher::initializePayload(isBirth);
}

bool Node::isActive()
{
    if (!enabled)
    {
        return false;
    }
    connect();
    SparkplugClient *activeClient = getActiveClient();
    if (activeClient == NULL)
    {
        return false;
    }
    if (activeClient->getState() != CONNECTED)
    {
        return false;
    }
    return true;
}