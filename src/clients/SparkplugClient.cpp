/*
 * File: SparkplugClient.cpp
 * Project: cpp_sparkplug
 * Created Date: Friday November 18th 2022
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

#include "clients/SparkplugClient.h"
#include <iostream>

using namespace std;

#define SPARKPLUGCLIENT_LOGGER cout << "Sparkplug Client: "

SparkplugClient::SparkplugClient()
{
}

SparkplugClient::SparkplugClient(ClientEventHandler *handler, ClientOptions *options) : options(options), handler(handler)
{
}

SparkplugClient::~SparkplugClient()
{
}

int SparkplugClient::configure(ClientTopicOptions *topics)
{
    int returnCode;

    if (topics == NULL)
    {
        SPARKPLUGCLIENT_LOGGER "Error: Client topics are not defined.\n";
        return -1;
    }

    this->topics = topics;

    returnCode = configureClient(options);

    if (returnCode < 0)
    {
        SPARKPLUGCLIENT_LOGGER "Error configuring client\n";
        return returnCode;
    }

    configured = true;
    return 0;
}

ClientEventHandler *SparkplugClient::getHandler()
{
    return handler;
}

int SparkplugClient::activate()
{
    if (getPrimary())
    {
        // Already primary
        return 0;
    }

    setPrimary(true);

    int returnCode;
    returnCode = subscribeToCommands();

    if (returnCode < 0)
    {
        // TODO: Error
        setPrimary(false);
        return returnCode;
    }

    return 0;
}

int SparkplugClient::deactivate()
{
    if (!getPrimary())
    {
        // Already not primary
        return 0;
    }
    int returnCode;

    returnCode = unsubscribeToCommands();

    if (returnCode < 0)
    {
        // TODO: Error
        return returnCode;
    }

    setPrimary(false);

    return 0;
}

int SparkplugClient::connect()
{
    if (getState() != DISCONNECTED)
    {
        // TODO: Already Connected
        return 0;
    }

    if (!configured)
    {
        SPARKPLUGCLIENT_LOGGER << "Cannot connect client as it has not been configured\n";
        return -1;
    }

    int returnCode;

    returnCode = clientConnect();

    if (returnCode < 0)
    {
        setState(DISCONNECTED);
        // TODO: Connect Failure
        return returnCode;
    }
    return 0;
}

int SparkplugClient::disconnect()
{
    if (getState() == DISCONNECTED)
    {
        // TODO: Not connected
        return 0;
    }

    int returnCode;

    returnCode = clientDisconnect();

    if (returnCode < 0)
    {
        // TODO: Disconnect Failure
        // return returnCode;
        SPARKPLUGCLIENT_LOGGER "Failed to disconnect, possibly already disconnected\n";
    }
    return 0;
}

ClientState SparkplugClient::getState()
{
    return state;
}

int SparkplugClient::encodePayload(org_eclipse_tahu_protobuf_Payload *payload, uint8_t **buffer)
{
    size_t length = MAX_BUFFER_LENGTH;
    *buffer = (uint8_t *)malloc(length * sizeof(uint8_t));
    size_t message_length = encode_payload(*buffer, length, payload);
    return message_length;
}

void SparkplugClient::destroyRequest(PublishRequest *publishRequest)
{
    free(publishRequest->topic);
    free(publishRequest);
}

void SparkplugClient::setState(ClientState state)
{
    this->state = state;
}

void SparkplugClient::setPrimary(bool isPrimary)
{
    this->isPrimary = isPrimary;
}

bool SparkplugClient::getPrimary()
{
    return isPrimary;
}

int SparkplugClient::processRequest(PublishRequest *publishRequest)
{
    if (getState() == DISCONNECTED)
    {
        SPARKPLUGCLIENT_LOGGER "Cannot publish payloads while disconnected\n";
        return -1;
    }

    setState(PUBLISHING_PAYLOAD);

    org_eclipse_tahu_protobuf_Payload *payload = publishRequest->publisher->getPayload(publishRequest->isBirth);

    uint8_t *buffer;
    int length = encodePayload(payload, &buffer);

    int returnCode = -1;

    if (length > 0)
    {
        returnCode = publishMessage(
            publishRequest->topic,
            buffer,
            length,
            &publishRequest->token);
    }

    free(buffer);
    free_payload(payload);
    free(payload);

    return returnCode;
}

void SparkplugClient::activated()
{
    if (!getPrimary())
    {
        return;
    }
    handler->onEvent(this, CLIENT_ACTIVE, nullptr);
}

void SparkplugClient::connected()
{
    setState(CONNECTED);
    handler->onEvent(this, CLIENT_CONNECTED, nullptr);
    if (topics->primaryHostTopic != NULL)
    {
        subscribeToPrimaryHost();
    }
}

void SparkplugClient::disconnected(char *cause)
{
    setState(DISCONNECTED);
    handler->onEvent(this, CLIENT_DISCONNECTED, nullptr);
}

void SparkplugClient::delivered(PublishRequest *publishRequest)
{
    handler->onEvent(this, CLIENT_DELIVERED, publishRequest->publisher);
    SparkplugClient::destroyRequest(publishRequest);
}

void SparkplugClient::undelivered(PublishRequest *publishRequest)
{
    handler->onEvent(this, CLIENT_UNDELIVERED, publishRequest->publisher);
    SparkplugClient::destroyRequest(publishRequest);
}

void SparkplugClient::messageReceived(const char *topicName, int topicLength, void *payload, int payloadLength)
{
    MessageEventStruct messageEvent = {
        topicName, topicLength, payload, payloadLength
    };
    handler->onEvent(this, CLIENT_MESSAGE, &messageEvent);
}

void SparkplugClient::execute()
{
    if (connect() == 0)
    {
        sync();
    }
}