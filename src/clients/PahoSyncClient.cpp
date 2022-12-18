/*
 * File: PahoSyncClient.cpp
 * Project: sparkplug_c
 * Created Date: Tuesday December 13th 2022
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

#include "clients/PahoSyncClient.h"
#include <iostream>

#define QOS 1

using namespace std;

#define PAHOSYNCCLIENT_LOGGER cout << "Paho Client: "

int PahoSyncClient::clientConnect()
{
    if (getState() != DISCONNECTED)
    {
        return 0;
    }

    int returnCode;
    setState(CONNECTING);
    returnCode = MQTTClient_connect(client, &connectionOptions);

    if (returnCode != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to connect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        onDisconnect(NULL);
        return returnCode;
    }
    connected();
    return 0;
}

int PahoSyncClient::clientDisconnect()
{
    setState(DISCONNECTING);

    int returnCode;
    if ((returnCode = MQTTClient_disconnect(client, 0)) != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to disconnect, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        return returnCode;
    }
    setState(DISCONNECTED);
    return 0;
}

int PahoSyncClient::subscribeToPrimaryHost()
{
    int returnCode;

    returnCode = MQTTClient_subscribe(client, topics->primaryHostTopic, QOS);

    if (returnCode != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoSyncClient::subscribeToCommands()
{
    int returnCode;

    char *const commandTopics[2] = {topics->nodeCommandTopic, topics->deviceCommandTopic};
    int qos[] = {QOS, QOS};

    returnCode = MQTTClient_subscribeMany(client, 2, commandTopics, qos);

    if (returnCode != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to subscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Subscribe Failure
        return returnCode;
    }

    activated();

    return 0;
}

int PahoSyncClient::unsubscribeToCommands()
{
    int returnCode;

    char *const commandTopics[2] = {topics->nodeCommandTopic, topics->deviceCommandTopic};

    returnCode = MQTTClient_unsubscribeMany(client, 2, commandTopics);

    if (returnCode != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to unsubscribe, return code " << returnCode << "\n";
        returnCode = EXIT_FAILURE;
        // TODO: Unsubscribe Failure
        return returnCode;
    }

    return 0;
}

int PahoSyncClient::publishMessage(const char *topic, uint8_t *buffer, size_t length, DeliveryToken *token)
{
    int returnCode = MQTTClient_publish(client, topic, length, buffer, QOS, 0, token);

    if (returnCode == MQTTCLIENT_SUCCESS)
    {
        
    }
    else
    {
        onDeliveryFailure(*token);
    }
    return returnCode;
}
int PahoSyncClient::configureClient(ClientOptions *options)
{
    int returnCode;

    returnCode = MQTTClient_create(
        &client, options->address, options->clientId,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if (returnCode != MQTTCLIENT_SUCCESS)
    {
        PAHOSYNCCLIENT_LOGGER "Failed to create client";
        // TODO: Client Failure
        return EXIT_FAILURE;
    }

    connectionOptions = MQTTClient_connectOptions_initializer;

    connectionOptions.username = options->username;
    connectionOptions.password = options->password;
    connectionOptions.connectTimeout = options->connectTimeout;
    connectionOptions.keepAliveInterval = options->keepAliveInterval;
    connectionOptions.retryInterval = 0;
    connectionOptions.cleansession = 1;

    will.qos = 0;
    will.retained = 0;
    will.topicName = topics->nodeDeathTopic;
    will.message = NULL;

    connectionOptions.will = &will;

    return 0;
}

PahoSyncClient::~PahoSyncClient()
{
    MQTTClient_destroy(&client);
}

void PahoSyncClient::sync()
{
    MQTTClient_message *message;
    char *topic;
    int topicLength = 0, result;

    if (!publishQueue.empty())
    {
        PublishRequest *publishRequest = publishQueue.front();
        result = MQTTClient_waitForCompletion(client, publishRequest->token, 1);
        if (result == MQTTCLIENT_SUCCESS)
        {
            publishQueue.pop();
            delivered(publishRequest);
            publishFromQueue();
        }
        else if (result == MQTTCLIENT_DISCONNECTED)
        {
            onDisconnect(NULL);
            return;
        }

        int *tokens;

        result = MQTTClient_getPendingDeliveryTokens(client, &tokens);

        if (tokens != NULL)
        {
            if (tokens[0] == -1)
            {
                // We have no inflight tokens, yet we have a publish queue.
                // Enable publishing
                publishFromQueue();
            }

            free(tokens);

        }
        else if (result == MQTTCLIENT_DISCONNECTED)
        {
            onDisconnect(NULL);
            return;
        }
    }

    while (true)
    {
        result = MQTTClient_receive(client, &topic, &topicLength, &message, 0);
        if (result == MQTTCLIENT_SUCCESS || result == MQTTCLIENT_TOPICNAME_TRUNCATED)
        {
            if (message != NULL)
            {
                messageReceived(topic, topicLength, message->payload, message->payloadlen);
                MQTTClient_freeMessage(&message);
                free(topic);
            }
            else
            {
                // Loop ended
                break;
            }
        }
        else if (result == MQTTCLIENT_DISCONNECTED)
        {
            onDisconnect(NULL);
            break;
        }
        else
        {
            // Error
            break;
        }
    }
}


bool PahoSyncClient::isConnected()
{
    return MQTTClient_isConnected(client);
}