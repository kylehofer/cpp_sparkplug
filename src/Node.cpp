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

#include "metrics/CallbackMetric.h"
#include "CommonTypes.h"

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

#define NODE_CONTROL_REBIRTH_NAME "Node Control/Rebirth"
#define NODE_CONTROL_REBOOT_NAME "Node Control/Reboot"
#define NODE_CONTROL_NEXT_SERVER_NAME "Node Control/Next Server"

#ifdef DEBUGGING
#define LOGGER(format, ...) \
    printf("Node: ");       \
    printf(format, ##__VA_ARGS__)
#else
#define LOGGER(out, ...)
#endif

Node::Node() : Node(NULL)
{
}

Node::Node(NodeOptions *options) : Publishable()
{
    if (options != NULL)
    {
        Publishable::setPublishPeriod(options->publishPeriod);
        configureTopics(options->groupId, options->nodeId, options->primaryHost);

        uint8_t commands = options->enabledCommands;

        if (commands & NODE_CONTROL_REBIRTH)
        {
            nodeBirthMetric = new CallbackMetric(
                NODE_CONTROL_REBIRTH_NAME,
                false,
                METRIC_DATA_TYPE_BOOLEAN,
                [this](CallbackMetric *metric, org_eclipse_tahu_protobuf_Payload_Metric *payload)
                {
                    if (payload->value.boolean_value)
                    {
                        publishBirth();
                    }
                });
            addMetric((Metric *)nodeBirthMetric);
        }
    }
}

Node::~Node()
{
    for (auto client : clients)
    {
        delete client;
    }
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
        if (clientTopics.nodeDeathTopic != NULL)
        {
            free(clientTopics.nodeDeathTopic);
        }
    }
    if (nodeBirthMetric != NULL)
    {
        delete nodeBirthMetric;
    }

#ifdef _GLIBCXX_HAS_GTHREADS
    delete queueMutex;
#endif
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
    publish(this, true);
    for_each(devices.begin(), devices.end(), [this](Device *device)
             { publish((Publishable *)device, true); });
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

        char *nodeCommandTopic, *deviceCommandTopic, *primaryHostTopic, *nodeDeathTopic;

        // 3 slashes, 4 for NCMD, 1 for null character
        miscSize = 3 + 4 + 1;
        nodeCommandTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);
        sprintf(nodeCommandTopic, NODE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, NCMD, nodeId);

        // 3 slashes, 6 for NDEATH, 1 for null character
        miscSize = 3 + 6 + 1;
        nodeDeathTopic = (char *)malloc(nameSpaceSize + groupIdSize + nodeIdSize + miscSize);
        sprintf(nodeDeathTopic, NODE_TOPIC_BUILDER, SPARKPLUG_NAMESPACE, groupId, NDEATH, nodeId);

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
            nodeDeathTopic,
            deviceCommandTopic,
            primaryHostTopic};

        topicsConfigured = true;
    }
}

int Node::requestPublish(Publishable *publishable, bool isBirth)
{
    if (this == publishable)
    {
        return publish(publishable, isBirth);
    }
    else if (any_of(devices.begin(), devices.end(), [publishable](Device *device)
                    { return publishable == (Publishable *)device; }))
    {
        return publish(publishable, isBirth);
    }
    return 0;
}

#define PREPARE_TOPIC(dest, src, ...)       \
    {                                       \
        char buffer[MAX_TOPIC_LENGTH];      \
        sprintf(buffer, src, __VA_ARGS__);  \
        size_t length = strlen(buffer) + 1; \
        dest = (char *)malloc(length);      \
        strcpy(topic, buffer);              \
    }

int Node::publish(Publishable *publishable, bool isBirth)
{
    if (!publishable->canPublish() && !isBirth)
    {
        return 0;
    }

    publishable->publishing();

    char *topic;
    if (publishable == this)
    {
        PREPARE_TOPIC(topic, nodeBaseTopic, isBirth ? NBIRTH : NDATA);
    }
    else
    {
        PREPARE_TOPIC(topic, deviceBaseTopic, isBirth ? DBIRTH : DDATA, publishable->getName());
    }

    PublishRequest *publishRequest = (PublishRequest *)malloc(sizeof(PublishRequest));
    publishRequest->isBirth = isBirth;
    publishRequest->publisher = (Publishable *)publishable;
    publishRequest->topic = topic;
    publishRequest->token = -1;

    return getActiveClient()->request(publishRequest);
}

void Node::setClientMode(SparkplugClientMode mode)
{
    this->hostMode = mode;
}

SparkplugClientMode Node::getClientMode()
{
    return hostMode;
}

void Node::setActiveClient(SparkplugClient *activeClient)
{
    bool isActiveClientNew = activeClient != this->activeClient;
    bool isActiveClientNull = this->activeClient == NULL;
    bool isNextClientNull = activeClient == NULL;

    if (!isActiveClientNew)
    {
        return;
    }

    if (!isActiveClientNull)
    {
        this->activeClient->deactivate();
    }
    this->activeClient = activeClient;

    if (!isNextClientNull)
    {
        publishBirth();
    }
}

void Node::activateClient(SparkplugClient *client)
{
    if (client == this->activeClient)
    {
        return;
    }

    if (client != NULL)
    {
        client->activate();
    }
}

void Node::deactivateClient(SparkplugClient *client)
{
    bool isActiveClient;
    isActiveClient = client != this->activeClient;

    if (isActiveClient)
    {
        return;
    }

    setActiveClient(NULL);
}

SparkplugClient *Node::getActiveClient()
{
    return activeClient;
}

void Node::sync()
{
    for (auto client : clients)
    {
        client->execute();
    }

    processEvents();
}

int32_t Node::execute(int32_t executeTime)
{
    if (!enabled)
    {
        cout << "Cannot execute as the node has not been enabled\n";
        return -1;
    }

    sync();

    if (!isActive())
    {
        return EXECUTE_IDLE_DELAY;
    }

    int32_t nextExecute = 0xFFFF;
    nextExecute = min(update(executeTime), nextExecute);
    if (canPublish())
    {
        publish(this);
    }

    for_each(devices.begin(), devices.end(),
             [this, executeTime, &nextExecute](Device *device)
             {
                 nextExecute = min(device->update(executeTime), nextExecute);

                 if (device->canPublish())
                 {
                     publish((Publishable *)device);
                 }
             });

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
            // Idea is to run this on its own thread
            return;
        }

        nextExecute = execute(nextExecute);
        if defined (__linux__)
            std::this_thread::sleep_for(std::chrono::seconds(nextExecute));
#endif
    }
}

SparkplugClient *Node::addClient(SparkplugClient *client)
{
    clients.push_back(client);
    return client;
}

int Node::nextServer()
{
    return 0;
}

void Node::addDevice(Device *device)
{
    devices.push_front(device);
}

int Node::onMessage(SparkplugClient *client, const char *topicName, int topicLength, void *payload, int payloadLength)
{
    if (client == getActiveClient())
    {
        if (strstr(topicName, NCMD) != NULL)
        {
            if (strcmp(topicName, clientTopics.nodeCommandTopic) == 0)
            {
                Publishable::handleCommand(this, payload, payloadLength);
            }
        }
        else if (strstr(topicName, DCMD) != NULL)
        {
            if (devices.empty())
            {
                return 0;
            }

            // Take off 1 for the "+" wildcard
            size_t deviceCommandPefixLength = strlen(clientTopics.deviceCommandTopic) - 1;
            size_t deviceNameLength = topicLength - deviceCommandPefixLength;
            if (deviceNameLength > 0)
            {
                // Add 1 for null character
                char *deviceName = (char *)malloc(deviceNameLength + 1);
                memcpy(deviceName, topicName + deviceCommandPefixLength, deviceNameLength);

                forward_list<Device *>::iterator result;

                result = find_if(devices.begin(), devices.end(), [deviceName](Device *device)
                                 { return (strcmp(device->getName(), deviceName) == 0); });

                if (result != devices.end())
                {
                    Publishable *publishable;
                    publishable = (Publishable *)*result;
                    // Handle Device Command
                    publishable->handleCommand(this, payload, payloadLength);
                }

                free(deviceName);
            }
        }
    }

    if (clientTopics.primaryHostTopic != NULL && strcmp(topicName, clientTopics.primaryHostTopic) == 0)
    {
        if (strncmp((char *)payload, "ONLINE", payloadLength) == 0)
        {
            // Primary Host Online
            if (getActiveClient() != client)
            {
                activateClient(client);
            }
        }
        else if (strncmp((char *)payload, "OFFLINE", payloadLength) == 0)
        {
            if (getActiveClient() == client)
            {
                deactivateClient(client);
            }
        }
    }
    return 0;
}

void Node::stop()
{
    for (auto client : clients)
    {
        client->deactivate();
        client->disconnect();
    }
}

bool Node::isActive()
{
    if (!enabled)
    {
        return false;
    }

    SparkplugClient *activeClient = getActiveClient();
    if (activeClient == NULL)
    {
        return false;
    }
    return activeClient->isConnected();
}

void *copyBuffer(void *source, size_t length)
{
    void *buffer;
    buffer = malloc(length);
    memcpy(buffer, source, length);
    return buffer;
}

void Node::onEvent(SparkplugClient *client, EventType event, void *data)
{
    void *eventData = nullptr;

    switch (event)
    {
    case CLIENT_MESSAGE:
    {
        // Copy the incoming data into the queue
        MessageEventStruct *incomingData, *queueData;

        incomingData = (MessageEventStruct *)data;
        queueData = (MessageEventStruct *)malloc(sizeof(MessageEventStruct));

        queueData->topicName = (char *)copyBuffer((void *)incomingData->topicName, incomingData->topicLength + 1);
        queueData->topicLength = incomingData->topicLength;

        queueData->payload = copyBuffer(incomingData->payload, incomingData->payloadLength);
        queueData->payloadLength = incomingData->payloadLength;

        eventData = queueData;
    }
    break;
    case CLIENT_CONNECTED:
        break;
    case CLIENT_DISCONNECTED:
        break;
    case CLIENT_ACTIVE:
        break;
    case CLIENT_DEACTIVE:
        break;
    case CLIENT_DELIVERED:
    case CLIENT_UNDELIVERED:
        eventData = data;
        break;
    default:
        break;
    }

#ifdef _GLIBCXX_HAS_GTHREADS
    queueMutex->lock();
#endif

    eventQueue.push_back({client, event, eventData});

#ifdef _GLIBCXX_HAS_GTHREADS
    queueMutex->unlock();
#endif
}

void Node::processEvents()
{
    while (!eventQueue.empty())
    {

#ifdef _GLIBCXX_HAS_GTHREADS
        queueMutex->lock();
#endif

        ClientEventData eventData = eventQueue.front();
        eventQueue.pop_front();

#ifdef _GLIBCXX_HAS_GTHREADS
        queueMutex->unlock();
#endif

        switch (eventData.eventType)
        {
        case CLIENT_DELIVERED:
        {
            Publishable *publishable;
            publishable = (Publishable *)eventData.data;
            publishable->published();
        }
        break;
        case CLIENT_UNDELIVERED:
        {
            Publishable *publishable;
            publishable = (Publishable *)eventData.data;
            publishable->published();
        }
        break;
        case CLIENT_MESSAGE:
        {
            MessageEventStruct *messageData;
            messageData = (MessageEventStruct *)eventData.data;

            onMessage(
                eventData.client,
                messageData->topicName, messageData->topicLength,
                messageData->payload, messageData->payloadLength);

            // We copy data to our queue, so we must free it
            free(messageData->payload);
            free((void *)messageData->topicName);
            free(messageData);
        }
        break;
        case CLIENT_CONNECTED:
            if (getClientMode() == SINGLE)
            {
                activateClient(eventData.client);
            }
            break;
        case CLIENT_DISCONNECTED:
            break;
        case CLIENT_ACTIVE:
            setActiveClient(eventData.client);
            break;
        case CLIENT_DEACTIVE:
            break;
        default:
            break;
        }
    }
}