/*
 * File: TahuBroker.cpp
 * Project: sparkplug_c
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

#include "TahuBroker.h"
#include <iostream>


#define QOS 1

using namespace std;

#define TAHUBROKER_LOGGER cout << "Tahu Broker: "

/**
 * @brief
 *
 * @param context
 * @param token
 */
static void deliveryComplete(void *context, MQTTAsync_token token)
{
    TAHUBROKER_LOGGER "deliveryComplete!\n";
    TahuBroker *broker = (TahuBroker *)context;
    broker->onDelivery(token);
}

static void deliveryFailure(void *context, MQTTAsync_failureData *response)
{
    TAHUBROKER_LOGGER "deliveryFailure!\n";
    TahuBroker *broker = (TahuBroker *)context;

    broker->onDeliveryFailure(response->token);
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
    TahuBroker *broker = (TahuBroker *)context;
    return broker->onMessage(topicName, topicLen, message);
}

/**
 * @brief
 *
 * @param context
 * @param cause
 */
static void connectionLost(void *context, char *cause)
{
    TAHUBROKER_LOGGER "Connection Lost!\n";
    TahuBroker *broker = (TahuBroker *)context;
    broker->onDisconnect(cause);
}

void connectionFailure(void* context, MQTTAsync_failureData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
    broker->onConnectFailure(response->code);
}

static void connectionSuccess(void* context, MQTTAsync_successData* response)
{
    TAHUBROKER_LOGGER "Connection Success\n";
    TahuBroker *broker = (TahuBroker *)context;
    broker->onConnect();
}

static void onSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
    // response->alt.qos.
}

static void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
}

static void onCommandSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
    
    // response->alt.qos.
}

static void onCommandSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
}

static void onSubscribeHostSuccess(void* context, MQTTAsync_successData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
    // response->alt.qos.
}


static void onSubscribeHostFailure(void* context, MQTTAsync_failureData* response)
{
    TahuBroker *broker = (TahuBroker *)context;
}

int TahuBroker::clientConnect()
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
        TAHUBROKER_LOGGER "Failed to connect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int TahuBroker::clientDisconnect()
{
    MQTTAsync_disconnectOptions disconnectOptions = MQTTAsync_disconnectOptions_initializer;

    disconnectOptions.timeout = 10000;

    setState(DISCONNECTING);

    int returnCode;
    if ((returnCode = MQTTAsync_disconnect(client, &disconnectOptions)) != MQTTASYNC_SUCCESS)
    {
        TAHUBROKER_LOGGER "Failed to disconnect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    return 0;
}

int TahuBroker::subscribeToPrimaryHost()
{
    int returnCode;

    MQTTAsync_responseOptions subscribeOptions = MQTTAsync_responseOptions_initializer;
    subscribeOptions.context = this;
    subscribeOptions.onSuccess = onSubscribeSuccess;
    subscribeOptions.onFailure = onSubscribeFailure;
    
    returnCode = MQTTAsync_subscribe(client, topics->primaryHostTopic, QOS, &subscribeOptions);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        TAHUBROKER_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0; 
}

int TahuBroker::subscribeToCommands()
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
        TAHUBROKER_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int TahuBroker::unsubscribeToCommands()
{
    int returnCode;

    char* const commandTopics[2] = { topics->nodeCommandTopic, topics->deviceCommandTopic };
    int qos[] = { QOS, QOS };

    returnCode = MQTTAsync_unsubscribeMany(client, 2, commandTopics, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        TAHUBROKER_LOGGER "Failed to unsubscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Unsubscribe Failure
        return returnCode;
    }

    return 0;
}

int TahuBroker::publishMessage(const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token)
{
    MQTTAsync_responseOptions responseOptions = MQTTAsync_responseOptions_initializer;

    responseOptions.onFailure = deliveryFailure;
    // responseOptions.onSuccess = onSend;
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

int TahuBroker::configureClient(BrokerOptions* options)
{
    int returnCode;

    returnCode = MQTTAsync_create(
        &client, options->address, options->clientId,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if (returnCode != MQTTASYNC_SUCCESS)
    {
        TAHUBROKER_LOGGER "Failed to create client";
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

TahuBroker::TahuBroker() : SparkplugBroker() { }

TahuBroker::TahuBroker(BrokerEventHandler *handler, BrokerOptions* options) : SparkplugBroker(handler, options) { }

TahuBroker::~TahuBroker()
{
    SparkplugBroker::~SparkplugBroker();
}

bool TahuBroker::setPrimary(bool isPrimary)
{
    SparkplugBroker::setPrimary(isPrimary);

    if (!isPrimary)
    {
        dumpQueue();
    }
}

void TahuBroker::publishFromQueue()
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

void TahuBroker::dumpQueue()
{
    while (!publishQueue.empty())
    {
        SparkplugBroker::destroyRequest(publishQueue.front());
        publishQueue.pop();
    }
}

int TahuBroker::onMessage(char *topicName, int topicLen, MQTTAsync_message *message)
{
    if (handler != NULL)
    {
        SparkplugMessage* sparkplugMessage = (SparkplugMessage*) malloc(sizeof(SparkplugMessage));
        handler->onMessage(this, topicName, topicLen, sparkplugMessage);
        free(sparkplugMessage);
        MQTTAsync_freeMessage(&message);
    }
    return 1;
}

void TahuBroker::onDelivery(DeliveryToken token)
{
    PublishRequest* publishRequest = publishQueue.front();

    if (publishRequest->token == token)
    {
        publishQueue.pop();
        if (handler != NULL)
        {
            handler->onDelivery(this, publishRequest);
        }
        SparkplugBroker::destroyRequest(publishRequest);

        publishFromQueue();
    }
    else
    {
        TAHUBROKER_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void TahuBroker::onDeliveryFailure(DeliveryToken token)
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
            SparkplugBroker::destroyRequest(publishRequest);
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
        TAHUBROKER_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void TahuBroker::onConnect()
{
    int returnCode;
    int** tokens;
    returnCode = MQTTAsync_getPendingTokens(client, tokens);
    connected();
}

void TahuBroker::onDisconnect(char *cause)
{
    disconnected(cause);
}

void TahuBroker::onConnectFailure(int responseCode)
{
    TAHUBROKER_LOGGER "Failed to connected. Response Code: " << responseCode << "\n";
    setState(DISCONNECTED);
}

int TahuBroker::requestPublish(PublishRequest* publishRequest)
{
    publishQueue.push(publishRequest);
    if (publishQueue.size() >= 1 && getState() == CONNECTED)
    {
        publishFromQueue();
    }
    return 0;
}
