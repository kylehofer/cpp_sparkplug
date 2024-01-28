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

#include "clients/PahoClient.h"
#include "CommonTypes.h"
#include <iostream>

#define QOS 1

using namespace std;

#define PAHOCLIENT_LOGGER cout << "Paho Async Client: "

PahoClient::~PahoClient()
{
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
    if (!isConnected())
    {
        return;
    }
    //    MQTTAsync_isComplete
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

void PahoClient::dumpQueue()
{
    while (!publishQueue.empty())
    {
        undelivered(publishQueue.front());
        publishQueue.pop();
    }
}

void PahoClient::onDeliveryFailure(DeliveryToken token)
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
        PAHOCLIENT_LOGGER "Oh no, we have a publish without a correct token\n";
    }
}

void PahoClient::onDisconnect(char *cause)
{
    dumpQueue();
    disconnected(cause);
}

int PahoClient::request(PublishRequest *publishRequest)
{
    publishQueue.push(publishRequest);
    if (publishQueue.size() == 1 && isConnected())
    {
        publishFromQueue();
    }
    return 0;
}
