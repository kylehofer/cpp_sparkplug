/*
 * File: CppMqttClient.cpp
 * Project: cpp_sparkplug
 * Created Date: Monday June 12th 2023
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2023 Kyle Hofer
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

#include "CppMqttClient.h"
#include "CommonTypes.h"
#include <iostream>
#include <string>

using namespace std;
using namespace PicoMqtt;

#define QOS QoS::ZERO

#ifdef DEBUGGING
#define LOGGER(format, ...)    \
    printf("CppMqttClient: "); \
    printf(format, ##__VA_ARGS__)
#else
#define LOGGER(out, ...)
#endif
void CppMqttClient::publishFromQueue()
{
    if (!isConnected())
    {
        return;
    }
    if (getPrimary() && publishQueue.size() > 0)
    {
        processRequest(publishQueue.front());
    }
    else
    {
        if (getState() == PUBLISHING_PAYLOAD)
        {
            setState(CONNECTED);
        }
    }
}

void CppMqttClient::dumpQueue()
{
    while (!publishQueue.empty())
    {
        undelivered(publishQueue.front());
        publishQueue.pop();
    }
}

void CppMqttClient::setPrimary(bool isPrimary)
{
    SparkplugClient::setPrimary(isPrimary);

    if (!isPrimary)
    {
        dumpQueue();
    }
}

int CppMqttClient::clientConnect()
{
    Uri uri = options->address;

    const string protocol = uri.getProtocol();

    const string tcpProtocol("tcp://");

    int port = uri.getPort();

    if (protocol.compare(tcpProtocol) != 0)
    {
        // TODO: Error bad protocol
    }

    int returnCode;

    returnCode = client.connect(
        uri.getAddress().c_str(),
        port, options->connectTimeout);

    if (returnCode == -1)
    {
        LOGGER("Failed to initialize connection, return code %d\n", returnCode);
        returnCode = -1;
        return returnCode;
    }
    return 0;
}

int CppMqttClient::clientDisconnect()
{
    if (!isConnected())
    {
        return -1;
    }

    setState(DISCONNECTING);

    int returnCode = client.disconnect(ReasonCode::DISCONNECT_WITH_WILL_MESSAGE);

    if (returnCode == -1)
    {
        LOGGER("Failed to disconnect, return code %d\n", returnCode);
        returnCode = EXIT_FAILURE;
    }

    return 0;
}

int CppMqttClient::subscribeToPrimaryHost()
{
    int returnCode;

    SubscribePayload primaryHostPayload;

    EncodedString primaryHostTopic(topics->primaryHostTopic.c_str(), topics->primaryHostTopic.size());

    primaryHostPayload.setTopic(primaryHostTopic);

    returnCode = client.subscribe(primaryHostPayload);

    if (returnCode == -1)
    {
        LOGGER("Failed to subscribe to %s, return code %d\n", topics->primaryHostTopic.c_str(), returnCode);
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int CppMqttClient::subscribeToCommands()
{
    int returnCode;

    SubscribePayload nodeCommandPayload, deviceCommandPayload;

    EncodedString encodedNodeTopic(topics->nodeCommandTopic.c_str(), topics->nodeCommandTopic.size());
    EncodedString encodedDeviceTopic(topics->deviceCommandTopic.c_str(), topics->deviceCommandTopic.size());

    nodeCommandPayload.setTopic(encodedNodeTopic);
    deviceCommandPayload.setTopic(encodedDeviceTopic);

    returnCode = client.subscribe(nodeCommandPayload, deviceCommandPayload);

    if (returnCode == -1)
    {
        LOGGER("Failed to subscribe, return code %d\n", returnCode);
        // TODO: Subscribe Failure
        return returnCode;
    }

    subscriptionToken = returnCode;

    return 0;
}

int CppMqttClient::unsubscribeToCommands()
{
    return 0;
}

int CppMqttClient::publishMessage(const string &topic, uint8_t *buffer, size_t length, DeliveryToken *token)
{
    EncodedString encodedTopic(topic.c_str(), topic.size());

    Payload payload = Payload(buffer, length);

    *token = client.publish(encodedTopic, payload, QOS);

    if (*token < 0)
    {
        onDeliveryFailure(*token, 0);
    }
    return *token;
}

int CppMqttClient::configureClient(ClientOptions *options)
{
    this->options = options;

    client = MqttClient(getClient());
    client.setHandler(this);

    client.setClientId(options->clientId, strlen(options->clientId));

    if (options->username != NULL)
    {
        EncodedString username(options->username, strlen(options->username));
        EncodedString password(options->password, strlen(options->password));
        client.setUserName(username);
        client.setPassword(password);
    }

    client.setKeepAliveInterval(options->keepAliveInterval);
    client.setCleanStart(true);

    if (will)
    {
        delete will;
    }

    will = new WillProperties();

    will->setWillTopic(topics->nodeDeathTopic.c_str(), topics->nodeDeathTopic.size());

    uint8_t *buffer;

    size_t size = getWillPayload(&buffer);

    will->setBinaryPayload((const char *)buffer, size);

    free(buffer);

    client.setWill(will);

    return 0;
}

CppMqttClient::~CppMqttClient()
{
}

void CppMqttClient::sync()
{
    client.sync();
}

bool CppMqttClient::isConnected()
{
    return client.connected();
}

int CppMqttClient::request(PublishRequest *publishRequest)
{
    publishQueue.push(publishRequest);
    if (publishQueue.size() == 1 && isConnected())
    {
        publishFromQueue();
    }
    return 0;
}

void CppMqttClient::onConnectionSuccess()
{
    connected();
}

void CppMqttClient::onConnectionFailure(int reasonCode)
{
    LOGGER("Failed to connected. Response Code: %d\n", reasonCode);
    setState(DISCONNECTED);
}

void CppMqttClient::onDisconnection(ReasonCode reasonCode)
{
    dumpQueue();
    disconnected(reasonCode._to_string());
}

void CppMqttClient::onMessage(EncodedString &topic, Payload &payload)
{
    string strTopic(topic.data, topic.length);
    messageReceived(strTopic, payload.getData(), payload.size());
}

void CppMqttClient::onDeliveryComplete(Token token)
{
    PublishRequest *publishRequest = publishQueue.front();
    if (publishRequest->token == token)
    {
        publishQueue.pop();
        delivered(publishRequest);
        publishFromQueue();
    }
    else
    {
    }
}

// TODO: Handle based off reason code
void CppMqttClient::onDeliveryFailure(Token token, __attribute__((unused)) int reasonCode)
{
    if (publishQueue.empty())
    {
        return;
    }
    PublishRequest *publishRequest = publishQueue.front();

    if (publishRequest->token == token)
    {
        if (publishRequest->retryCount >= PUBLISH_RETRIES || !isConnected())
        {
            publishQueue.pop();
            undelivered(publishRequest);
            publishFromQueue();
        }
        else
        {
            publishRequest->retryCount++;
            processRequest(publishRequest);
        }
    }
    else
    {
    }
}

// TODO: Handle based off reason codes
void CppMqttClient::onSubscribeResult(Token token, __attribute__((unused)) vector<uint8_t> reasonCodes)
{
    if (subscriptionToken == token)
    {
        activated();
    }
}

// TODO: Implement
void CppMqttClient::onUnsubscribeResult(__attribute__((unused)) Token token, __attribute__((unused)) vector<uint8_t> reasonCodes)
{
}