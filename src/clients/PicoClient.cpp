/*
 * File: PicoClient.cpp
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

#include "clients/PicoClient.h"
#include "CommonTypes.h"
#include <iostream>
#include <string>

using namespace std;
using namespace PicoMqtt;

#define QOS QoS::ONE

#ifdef DEBUGGING
#define LOGGER(format, ...) \
    printf("PicoClient: "); \
    printf(format, ##__VA_ARGS__)
#else
#define LOGGER(out, ...)
#endif

void PicoClient::publishFromQueue()
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

void PicoClient::dumpQueue()
{
    while (!publishQueue.empty())
    {
        undelivered(publishQueue.front());
        publishQueue.pop();
    }
}

void PicoClient::setPrimary(bool isPrimary)
{
    SparkplugClient::setPrimary(isPrimary);

    if (!isPrimary)
    {
        dumpQueue();
    }
}

int PicoClient::clientConnect()
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
        port, 30000);

    if (returnCode == -1)
    {
        LOGGER("Failed to initialize connection, return code %d\n", returnCode);
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int PicoClient::clientDisconnect()
{
    if (!isConnected())
    {
        return -1;
    }

    setState(DISCONNECTING);

    int returnCode = client.disconnect(DISCONNECT_WITH_WILL_MESSAGE);

    if (returnCode == -1)
    {
        LOGGER("Failed to disconnect, return code %d\n", returnCode);
        returnCode = EXIT_FAILURE;
    }

    return 0;
}

int PicoClient::subscribeToPrimaryHost()
{
    int returnCode;

    SubscribePayload primaryHostPayload;

    EncodedString primaryHostTopic(topics->primaryHostTopic, strlen(topics->primaryHostTopic));

    primaryHostPayload.setTopic(primaryHostTopic);

    returnCode = client.subscribe(primaryHostPayload);

    if (returnCode == -1)
    {
        LOGGER("Failed to subscribe to %s, return code %d\n", topics->primaryHostTopic, returnCode);
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int PicoClient::subscribeToCommands()
{
    int returnCode;

    SubscribePayload nodeCommandPayload, deviceCommandPayload;

    EncodedString encodedNodeTopic(topics->nodeCommandTopic, strlen(topics->nodeCommandTopic));
    EncodedString encodedDeviceTopic(topics->deviceCommandTopic, strlen(topics->deviceCommandTopic));

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

int PicoClient::unsubscribeToCommands()
{
    return 0;
}

int PicoClient::publishMessage(const char *topic, uint8_t *buffer, size_t length, DeliveryToken *token)
{
    EncodedString encodedTopic(topic, strlen(topic));
    Payload payload(buffer, length);

    *token = client.publish(encodedTopic, payload, QOS);

    if (*token < 0)
    {
        onDeliveryFailure(*token, 0);
    }
    return *token;
}

int PicoClient::configureClient(ClientOptions *options)
{
    int returnCode;

    this->options = options;

    client = MqttClient((Client *)&tcpClient);
    client.setHandler(this);

    client.setClientId(options->clientId, strlen(options->clientId));

    if (options->username != NULL)
    {
        EncodedString username(options->username, strlen(options->username));
        EncodedString password(options->password, strlen(options->password));
        client.setUserName(username);
        client.setPassword(password);
    }

    client.setKeepAliveInterval(options->keepAliveInterval / 1000);
    client.setCleanStart(1);

    if (will != nullptr)
    {
        delete will;
    }

    will = new WillProperties();

    will->setWillTopic(topics->nodeDeathTopic, strlen(topics->nodeDeathTopic));

    uint8_t *buffer;

    size_t size = getWillPayload(&buffer);

    will->setBinaryPayload((const char *)buffer, size);

    free(buffer);

    client.setWill(will);

    return 0;
}

PicoClient::~PicoClient()
{
}

void PicoClient::sync()
{
    client.sync();
}

bool PicoClient::isConnected()
{
    return client.connected();
}

int PicoClient::request(PublishRequest *publishRequest)
{
    publishQueue.push(publishRequest);
    if (publishQueue.size() == 1 && isConnected())
    {
        publishFromQueue();
    }
    return 0;
}

void PicoClient::onConnectionSuccess()
{
    connected();
}

void PicoClient::onConnectionFailure(int reasonCode)
{
    LOGGER("Failed to connected. Response Code: %d\n", reasonCode);
    setState(DISCONNECTED);
}

void PicoClient::onDisconnection(int reasonCode)
{
    LOGGER("Disconnected. Reason Code: %d\n", reasonCode);
    char cause[] = "";
    dumpQueue();
    disconnected(cause);
}

void PicoClient::onMessage(EncodedString &topic, Payload &payload)
{
    messageReceived(topic.data, topic.length, payload.getData(), payload.size());
}

void PicoClient::onDeliveryComplete(Token token)
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

void PicoClient::onDeliveryFailure(Token token, int reasonCode)
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

void PicoClient::onSubscribeResult(Token token, vector<uint8_t> reasonCodes)
{
    if (subscriptionToken == token)
    {
        activated();
    }
}

void PicoClient::onUnsubscribeResult(Token token, vector<uint8_t> reasonCodes)
{
}