/*
 * File: PahoClient.cpp
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

#include "PahoClient.h"
#include <iostream>


#define QOS 1

using namespace std;

#define PAHOCLIENT_LOGGER cout << "Paho Client: "

/**
 * @brief
 *
 * @param context
 * @param token
 */
static void deliveryComplete(void *context, MQTTAsync_token token)
{
    PahoClient *client = (PahoClient *)context;
    client->onDelivery(token);
}

static void deliveryFailure(void *context, MQTTAsync_failureData *response)
{
    PahoClient *client = (PahoClient *)context;

    client->onDeliveryFailure(response->token);
}

/**
 * @brief
 *
 * @param context
 * @param topicName
 * @param topicLen
 * @param message
 * @return int
 */
static int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    PahoClient *client = (PahoClient *)context;
    return client->onMessage(topicName, topicLen, message);
}

/**
 * @brief
 *
 * @param context
 * @param cause
 */
static void connectionLost(void *context, char *cause)
{
    PahoClient *client = (PahoClient *)context;
    client->onDisconnect(cause);
}

void connectionFailure(void* context, MQTTAsync_failureData* response)
{
    PahoClient *client = (PahoClient *)context;
    client->onConnectFailure(response->code);
}

static void connectionSuccess(void* context, MQTTAsync_successData* response)
{
    PahoClient *client = (PahoClient *)context;
    client->onConnect();
}

static void onSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    PahoClient *client = (PahoClient *)context;
}

static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    PahoClient *client = (PahoClient *)context;
}

static void onCommandSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    PahoClient *client = (PahoClient *)context;

    client->onCommandSubscription();
}

static void onCommandSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    PahoClient *client = (PahoClient *)context;
}

static void onSubscribeHostSuccess(void* context, MQTTAsync_successData* response)
{
    PahoClient *client = (PahoClient *)context;
}


static void onSubscribeHostFailure(void* context, MQTTAsync_failureData* response)
{
    PahoClient *client = (PahoClient *)context;
}

int PahoClient::clientConnect()
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
        PAHOCLIENT_LOGGER "Failed to connect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int PahoClient::clientDisconnect()
{
    MQTTAsync_disconnectOptions disconnectOptions = MQTTAsync_disconnectOptions_initializer;

    disconnectOptions.timeout = 10000;

    setState(DISCONNECTING);

    int returnCode;
    if ((returnCode = MQTTAsync_disconnect(client, &disconnectOptions)) != MQTTASYNC_SUCCESS)
    {
        PAHOCLIENT_LOGGER "Failed to disconnect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int PahoClient::subscribeToPrimaryHost()
{
    int returnCode;

    MQTTAsync_responseOptions subscribeOptions = MQTTAsync_responseOptions_initializer;
    subscribeOptions.context = this;
    subscribeOptions.onSuccess = onSubscribeSuccess;
    subscribeOptions.onFailure = onSubscribeFailure;
    
    returnCode = MQTTAsync_subscribe(client, topics->primaryHostTopic, QOS, &subscribeOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0; 
}

int PahoClient::subscribeToCommands()
{
    int returnCode;

    MQTTAsync_responseOptions subscribeOptions = MQTTAsync_responseOptions_initializer;
    subscribeOptions.context = this;
    subscribeOptions.onSuccess = onCommandSubscribeSuccess;
    subscribeOptions.onFailure = onCommandSubscribeFailure;
    
    char* const commandTopics[2] = { topics->nodeCommandTopic, topics->deviceCommandTopic };
    int qos[] = { QOS, QOS };

    returnCode = MQTTAsync_subscribeMany(client, 2, commandTopics, qos, &subscribeOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoClient::unsubscribeToCommands()
{
    int returnCode;

    char* const commandTopics[2] = { topics->nodeCommandTopic, topics->deviceCommandTopic };
    int qos[] = { QOS, QOS };

    returnCode = MQTTAsync_unsubscribeMany(client, 2, commandTopics, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOCLIENT_LOGGER "Failed to unsubscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Unsubscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoClient::publishMessage(const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token)
{
    MQTTAsync_responseOptions responseOptions = MQTTAsync_responseOptions_initializer;

    responseOptions.onFailure = deliveryFailure;
    responseOptions.context = this;

    int returnCode = MQTTAsync_send(client, topic, length, buffer, QOS, 0, &responseOptions);

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

int PahoClient::configureClient(ClientOptions* options)
{
    int returnCode;

    returnCode = MQTTAsync_create(
        &client, options->address, options->clientId,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        PAHOCLIENT_LOGGER "Failed to create client";
        // TODO: Client Failure
        return EXIT_FAILURE;
    }

    returnCode = MQTTAsync_setCallbacks(
        client, this,
        connectionLost,
        messageArrived,
        deliveryComplete
    );

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

    return 0;
}

PahoClient::PahoClient() : SparkplugClient() { }

PahoClient::PahoClient(ClientEventHandler *handler, ClientOptions* options) : SparkplugClient(handler, options) { }

PahoClient::~PahoClient()
{
    MQTTAsync_destroy(&client);
    dumpQueue();
}

void PahoClient::setPrimary(bool isPrimary)
{
    SparkplugClient::setPrimary(isPrimary);

    if (!isPrimary)
    {
        dumpQueue();
    }
}

void PahoClient::publishFromQueue()
{
    if (getPrimary() && publishQueue.size() > 0)
    {
        publish(publishQueue.front());
    }
    else
    {
        if (getState() == PUBLISHING_PAYLOAD)
        {
            setState(CONNECTED);
        }
    }
}

void PahoClient::dumpQueue()
{
    while (!publishQueue.empty())
    {
        SparkplugClient::destroyRequest(publishQueue.front());
        publishQueue.pop();
    }
}

int PahoClient::onMessage(char *topicName, int topicLen, MQTTAsync_message *message)
{
    if (handler != NULL)
    {
        SparkplugMessage* sparkplugMessage = (SparkplugMessage*) malloc(sizeof(SparkplugMessage));
        sparkplugMessage->payload = message->payload;
        sparkplugMessage->payloadlen = message->payloadlen;
        handler->onMessage(this, topicName, topicLen, sparkplugMessage);
        free(sparkplugMessage);
        MQTTAsync_freeMessage(&message);
    }
    return 1;
}

void PahoClient::onDelivery(DeliveryToken token)
{
    PublishRequest* publishRequest = publishQueue.front();
    if (publishRequest->token == token)
    {
        publishQueue.pop();
        if (handler != NULL)
        {
            handler->onDelivery(this, publishRequest);
        }
        SparkplugClient::destroyRequest(publishRequest);
        publishFromQueue();
    }
    else
    {
        PAHOCLIENT_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void PahoClient::onDeliveryFailure(DeliveryToken token)
{
    if (getState() != CONNECTED)
    {
        return;
    }

    PublishRequest* publishRequest = publishQueue.front();

    if (publishRequest->token == token)
    {
        if (publishRequest->retryCount >= PUBLISH_RETRIES)
        {
            publishQueue.pop();
            {
                handler->onDelivery(this, publishRequest);
            }
            SparkplugClient::destroyRequest(publishRequest);
            publishFromQueue();
        }
        else
        {
            publishRequest->retryCount++;
            publish(publishRequest);
        }
    }
    else
    {
        PAHOCLIENT_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void PahoClient::onConnect()
{
    connected();
}

void PahoClient::onDisconnect(char *cause)
{
    disconnected(cause);
}

void PahoClient::onConnectFailure(int responseCode)
{
    PAHOCLIENT_LOGGER "Failed to connected. Response Code: " << responseCode << "\n";
    setState(DISCONNECTED);
}

int PahoClient::requestPublish(PublishRequest* publishRequest)
{
    publishQueue.push(publishRequest);
    if (publishQueue.size() == 1 && getState() == CONNECTED)
    {
        publishFromQueue();
    }
    return 0;
}

void PahoClient::onCommandSubscription()
{
    activated();
}