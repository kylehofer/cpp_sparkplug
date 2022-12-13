/*
 * File: TestClientUtility.cpp
 * Project: sparkplug_c
 * Created Date: Monday November 28th 2022
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

#include "utils/TestClientUtility.h"

#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>

#include <stdio.h>

using namespace std;

/**
 * @brief Callback when the moquitto client recieves a message
 *
 * @param client The client responsible
 * @param userdata
 * @param message
 */
static void onMessageCallback(struct mosquitto *client, void *userdata, const struct mosquitto_message *message);

/**
 * @brief Callback when the mosquitto client has connected.
 * Will subscribe to a wildcard Sparkplug topic so it can receive all sparkplug messages.
 *
 * @param client
 * @param userdata
 * @param result
 */
static void onConnectCallback(struct mosquitto *client, void *userdata, int result);

/**
 * @brief Callback when the wildcard Sparkplug topic has subscribed.
 * Will set the test client as ready
 *
 * @param client
 * @param userdata
 * @param mid
 * @param qosCount
 * @param grantedQos
 */
static void onSubscribeCallback(struct mosquitto *client, void *userdata, int mid, int qosCount, const int *grantedQos);

TestClientUtility::~TestClientUtility()
{
    if (client != NULL)
    {
        mosquitto_disconnect(client);
        mosquitto_destroy(client);
    }
}

void TestClientUtility::push(PayloadMessage message)
{
    messageQueue.push(message);
}

PayloadMessage TestClientUtility::pop()
{
    PayloadMessage message = messageQueue.front();
    messageQueue.pop();
    return message;
}

bool TestClientUtility::isReady()
{
    mosquitto_loop(client, 0, 1);
    return ready;
}

bool TestClientUtility::waitForReady(int maxRetries, int timePerLoop)
{
    int retries = 0;
    while (!isReady() && retries++ < maxRetries)
    {
        this_thread::sleep_for(std::chrono::milliseconds(timePerLoop));
    }
    return (retries < maxRetries);
}

bool TestClientUtility::hasData()
{
    mosquitto_loop(client, 0, 1);
    return !messageQueue.empty();
}

void TestClientUtility::waitForData(int maxRetries, int timePerLoop)
{
    int retries = 0;
    while (!hasData() && retries++ < maxRetries)
    {
        this_thread::sleep_for(std::chrono::milliseconds(timePerLoop));
    }
}

void TestClientUtility::cleanup()
{
    mosquitto_lib_cleanup();
}

void TestClientUtility::freePayload(PayloadMessage message)
{
    if (message.payload != NULL)
    {
        free(message.payload);
    }
    free(message.topic);
}

int TestClientUtility::publish(const char *topic, void *data, size_t dataLength, bool retain)
{
    int returnCode;
    returnCode = mosquitto_publish(client, NULL, topic, dataLength, data, 1, retain);

    if (returnCode == 0)
    {
        mosquitto_loop(client, -1, 1);
    }

    return returnCode;
}

int TestClientUtility::connect(const char *host, int port)
{
    ready = false;

    if (client == NULL)
    {
        mosquitto_lib_init();
        client = mosquitto_new("TestListener", true, NULL);
        if (!client)
        {
            return 1;
        }

        mosquitto_user_data_set(client, this);
        mosquitto_connect_callback_set(client, onConnectCallback);
        mosquitto_message_callback_set(client, onMessageCallback);
        mosquitto_subscribe_callback_set(client, onSubscribeCallback);
    }

    return mosquitto_connect(client, host, port, 60);
}

void TestClientUtility::setReady(bool ready)
{
    this->ready = ready;
}

static void onMessageCallback(struct mosquitto *client, void *userdata, const struct mosquitto_message *message)
{
    TestClientUtility *utility = (TestClientUtility *)userdata;
    void *payload = NULL;

    if (message->payloadlen > 0)
    {
        payload = malloc(message->payloadlen);
        memcpy(payload, message->payload, message->payloadlen);
    }

    PayloadMessage value = {
        strdup(message->topic),
        payload,
        message->payloadlen};

    utility->push(value);
}
static void onConnectCallback(struct mosquitto *client, void *userdata, int result)
{
    mosquitto_subscribe(client, NULL, "spBv1.0/#", 0);
}
static void onSubscribeCallback(struct mosquitto *client, void *userdata, int mid, int qosCount, const int *grantedQos)
{
    TestClientUtility *utility = (TestClientUtility *)userdata;
    utility->setReady(true);
}