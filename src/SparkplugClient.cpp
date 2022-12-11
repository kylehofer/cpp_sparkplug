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

#include "SparkplugClient.h"
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
    delete stateMutex;
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

    int returnCode;
    returnCode = subscribeToCommands();

    if (returnCode < 0)
    {
        // TODO: Error
        return returnCode;
    }

    setPrimary(true);

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
    lock_guard<mutex> lock(*stateMutex);
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
    lock_guard<mutex> lock(*stateMutex);
    this->state = state;
}

void SparkplugClient::setPrimary(bool isPrimary)
{
    lock_guard<mutex> lock(*stateMutex);
    this->isPrimary = isPrimary;
}

bool SparkplugClient::getPrimary()
{
    lock_guard<mutex> lock(*stateMutex);
    return isPrimary;
}

int SparkplugClient::processRequest(PublishRequest *publishRequest)
{
    lock_guard<mutex> lock(publishMutex);

    if (getState() == DISCONNECTED)
    {
        SPARKPLUGCLIENT_LOGGER "Cannot publish payloads while disconnected\n";
        return -1;
    }

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
    handler->onActive(this);
}

void SparkplugClient::connected()
{
    setState(CONNECTED);
    handler->onConnect(this);
    if (topics->primaryHostTopic != NULL)
    {
        subscribeToPrimaryHost();
    }
}

void SparkplugClient::disconnected(char *cause)
{
    setState(DISCONNECTED);
    handler->onDisconnect(this, cause);
}
