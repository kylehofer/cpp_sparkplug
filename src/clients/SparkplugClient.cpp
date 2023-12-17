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

#ifdef DEBUGGING
#define LOGGER(format, ...)       \
    printf("Sparkplug Client: "); \
    printf(format, ##__VA_ARGS__)
#else
#define LOGGER(out, ...)
#endif

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
        LOGGER("Error: Client topics are not defined.\n");
        return -1;
    }

    this->topics = topics;

    returnCode = configureClient(options);

    if (returnCode < 0)
    {
        LOGGER("Error configuring client\n");
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

size_t SparkplugClient::getWillPayload(uint8_t **buffer)
{
    incrementBdSeq();

    SimpleMetric bdSeqMetric = SimpleMetric("bdSeq", bdSeq, METRIC_DATA_TYPE_INT64);

    org_eclipse_tahu_protobuf_Payload *payload = initializePayload(false);

    bdSeqMetric.addToPayload(payload, true);

    int length = encodePayload(payload, buffer);

    free_payload(payload);
    free(payload);

    return length;
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
        LOGGER("Cannot connect client as it has not been configured\n");
        return -1;
    }

    LOGGER("Attempting to connect the client\n");

    int returnCode;

    returnCode = clientConnect();

    if (returnCode < 0)
    {
        setState(DISCONNECTED);
        // TODO: Connect Failure
        return returnCode;
    }

    setState(CONNECTING);
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
        LOGGER("Failed to disconnect, possibly already disconnected\n");
    }
    return 0;
}

ClientState SparkplugClient::getState()
{
    return state;
}

void SparkplugClient::resetSequence()
{
    payloadSequence = 0;
}

org_eclipse_tahu_protobuf_Payload *SparkplugClient::initializePayload(bool hasSeq)
{
    org_eclipse_tahu_protobuf_Payload *payload = (org_eclipse_tahu_protobuf_Payload *)malloc(sizeof(org_eclipse_tahu_protobuf_Payload));

    // Initialize payload
    memset(payload, 0, sizeof(org_eclipse_tahu_protobuf_Payload));
    payload->has_timestamp = true;
    payload->timestamp = get_current_timestamp();
    if (hasSeq)
    {
        LOGGER("Current Sequence Number: %u\n", payloadSequence);
        payload->seq = payloadSequence++;
        payload->has_seq = true;
    }
    return payload;
}

size_t SparkplugClient::encodePayload(org_eclipse_tahu_protobuf_Payload *payload, uint8_t **buffer)
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
        LOGGER("Cannot publish payloads while disconnected\n");
        return -1;
    }

    setState(PUBLISHING_PAYLOAD);

    org_eclipse_tahu_protobuf_Payload *payload = initializePayload();

    publishRequest->publisher->addToPayload(payload, publishRequest->isBirth);

    if (publishRequest->publisher->isNode() && publishRequest->isBirth)
    {
        SimpleMetric bdSeqMetric = SimpleMetric("bdSeq", bdSeq, METRIC_DATA_TYPE_INT64);
        bdSeqMetric.addToPayload(payload, true);
    }

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

void SparkplugClient::incrementBdSeq()
{
    bdSeq = bdSeq == 255 ? 0 : bdSeq + 1;
}

void SparkplugClient::disconnected(char *cause)
{
    LOGGER("Disconnected. Reason: %s.", cause);
    setState(DISCONNECTED);
    handler->onEvent(this, CLIENT_DISCONNECTED, nullptr);
}

void SparkplugClient::delivered(PublishRequest *publishRequest)
{
    LOGGER("Publish Request was delivered for %s on topic: %s\n", publishRequest->publisher->getName(), publishRequest->topic);
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
        topicName, topicLength, payload, payloadLength};
    handler->onEvent(this, CLIENT_MESSAGE, &messageEvent);
}

void SparkplugClient::execute()
{
    if (connect() == 0)
    {
        sync();
    }
}