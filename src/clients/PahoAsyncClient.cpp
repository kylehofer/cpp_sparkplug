/*
 * File: PahoAsyncClient.cpp
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

#include "clients/PahoAsyncClient.h"
#include "CommonTypes.h"
#include <iostream>

#define QOS 1

using namespace std;

#define PAHOASYNCCLIENT_LOGGER cout << "Paho Async Client: "

/**
 * @brief
 *
 * @param context
 * @param token
 */
static void deliveryComplete(void *context, MQTTAsync_token token)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;
    client->onDelivery(token);
}

static void deliveryFailure(void *context, MQTTAsync_failureData *response)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;

    client->onDeliveryFailure(response->token);
}

/**
 * @brief
 *
 * @param context
 * @param topicName
 * @param topicLength
 * @param message
 * @return int
 */
static int messageArrived(void *context, char *topicName, int topicLength, MQTTAsync_message *message)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;
    return client->onMessage(topicName, topicLength, message);
}

/**
 * @brief
 *
 * @param context
 * @param cause
 */
static void connectionLost(void *context, char *cause)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;
    client->onDisconnect(cause);
}

void connectionFailure(void *context, MQTTAsync_failureData *response)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;
    client->onConnectFailure(response->code);
}

static void connectionSuccess(void *context, MQTTAsync_successData *response)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;
    client->onConnect();
}

static void onSubscribeSuccess(void *context, MQTTAsync_successData *response)
{
    // PahoAsyncClient *client = (PahoAsyncClient *)context;
}

static void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    // PahoAsyncClient *client = (PahoAsyncClient *)context;
}

static void onCommandSubscribeSuccess(void *context, MQTTAsync_successData *response)
{
    PahoAsyncClient *client = (PahoAsyncClient *)context;

    client->onCommandSubscription();
}

static void onCommandSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    // PahoAsyncClient *client = (PahoAsyncClient *)context;
}

int PahoAsyncClient::clientConnect()
{
    if (getState() != DISCONNECTED)
    {
        return 0;
    }

    int returnCode;
    setState(CONNECTING);
    returnCode = MQTTAsync_connect(client, &connectionOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to connect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int PahoAsyncClient::clientDisconnect()
{
    MQTTAsync_disconnectOptions disconnectOptions = MQTTAsync_disconnectOptions_initializer;

    disconnectOptions.timeout = 0;

    setState(DISCONNECTING);

    int returnCode;
    if ((returnCode = MQTTAsync_disconnect(client, &disconnectOptions)) != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to disconnect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int PahoAsyncClient::subscribeToPrimaryHost()
{
    int returnCode;

    MQTTAsync_responseOptions subscribeOptions = MQTTAsync_responseOptions_initializer;
    subscribeOptions.context = this;
    subscribeOptions.onSuccess = onSubscribeSuccess;
    subscribeOptions.onFailure = onSubscribeFailure;

    returnCode = MQTTAsync_subscribe(client, topics->primaryHostTopic.c_str(), QOS, &subscribeOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoAsyncClient::subscribeToCommands()
{
    int returnCode;

    MQTTAsync_responseOptions subscribeOptions = MQTTAsync_responseOptions_initializer;
    subscribeOptions.context = this;
    subscribeOptions.onSuccess = onCommandSubscribeSuccess;
    subscribeOptions.onFailure = onCommandSubscribeFailure;

    char *const commandTopics[2] = {(char *)topics->nodeCommandTopic.c_str(), (char *)topics->deviceCommandTopic.c_str()};
    int qos[] = {QOS, QOS};

    returnCode = MQTTAsync_subscribeMany(client, 2, commandTopics, qos, &subscribeOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoAsyncClient::unsubscribeToCommands()
{
    int returnCode;

    char *const commandTopics[2] = {(char *)topics->nodeCommandTopic.c_str(), (char *)topics->deviceCommandTopic.c_str()};

    returnCode = MQTTAsync_unsubscribeMany(client, 2, commandTopics, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to unsubscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Unsubscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoAsyncClient::publishMessage(const std::string &topic, uint8_t *buffer, size_t length, DeliveryToken *token)
{
    MQTTAsync_responseOptions responseOptions = MQTTAsync_responseOptions_initializer;

    responseOptions.onFailure = deliveryFailure;
    responseOptions.context = this;

    int returnCode = MQTTAsync_send(client, topic.c_str(), length, buffer, QOS, 0, &responseOptions);

    if (returnCode == MQTTASYNC_SUCCESS)
    {
        *token = responseOptions.token;
    }
    else
    {
        // TODO: Async Failure
    }
    return returnCode;
}
int PahoAsyncClient::configureClient(ClientOptions *options)
{
    int returnCode;

    returnCode = MQTTAsync_create(
        &client, options->address.getAddress().c_str(), options->clientId,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOASYNCCLIENT_LOGGER "Failed to create client";
        // TODO: Client Failure
        return EXIT_FAILURE;
    }

    returnCode = MQTTAsync_setCallbacks(
        client, this,
        connectionLost,
        messageArrived,
        deliveryComplete);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", returnCode);
        // TODO: Callback Failure
        return EXIT_FAILURE;
    }

    connectionOptions = MQTTAsync_connectOptions_initializer;

    connectionOptions.context = this;
    connectionOptions.onSuccess = connectionSuccess;
    connectionOptions.onFailure = connectionFailure;

    connectionOptions.maxInflight = 10;

    connectionOptions.username = options->username;
    connectionOptions.password = options->password;
    connectionOptions.connectTimeout = options->connectTimeout;
    connectionOptions.keepAliveInterval = options->keepAliveInterval;
    connectionOptions.retryInterval = 0;
    connectionOptions.cleansession = 1;
    connectionOptions.automaticReconnect = true;

    will.qos = 0;
    will.retained = 0;
    will.topicName = topics->nodeDeathTopic.c_str();
    will.message = NULL;

    connectionOptions.will = &will;

    return 0;
}

PahoAsyncClient::~PahoAsyncClient()
{
    MQTTAsync_destroy(&client);
    dumpQueue();
}

int PahoAsyncClient::onMessage(char *topicName, int topicLength, MQTTAsync_message *message)
{
    {
        std::string stringTopic(topicName, topicLength);
        messageReceived(stringTopic, message->payload, message->payloadlen);
    }

    MQTTAsync_freeMessage(&message);
    return 1;
}

void PahoAsyncClient::onDelivery(DeliveryToken token)
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
        PAHOASYNCCLIENT_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void PahoAsyncClient::onConnect()
{
    connected();
}

void PahoAsyncClient::onConnectFailure(int responseCode)
{
    PAHOASYNCCLIENT_LOGGER "Failed to connected. Response Code: " << responseCode << "\n";
    setState(DISCONNECTED);
}

void PahoAsyncClient::onCommandSubscription()
{
    activated();
}

bool PahoAsyncClient::isConnected()
{
    return MQTTAsync_isConnected(client);
}