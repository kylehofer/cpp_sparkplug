/*
 * File: MockSparkplugClient.h
 * Project: cpp_sparkplug
 * Created Date: Sunday January 28th 2024
 * Author: Kyle Hofer
 *
 * MIT License
 *
 * Copyright (c) 2024 Kyle Hofer
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

#ifndef TESTS_MOCKS_MOCKSPARKPLUGCLIENT
#define TESTS_MOCKS_MOCKSPARKPLUGCLIENT

#include "gmock/gmock.h"
#include "clients/SparkplugClient.h"
#include <string>

class MockSparkplugClient : public SparkplugClient
{
private:
    bool connected = false;

protected:
public:
    MockSparkplugClient() : MockSparkplugClient(NULL, NULL){};
    MockSparkplugClient(ClientEventHandler *handler, ClientOptions *options) : SparkplugClient(handler, options){};

    MOCK_METHOD(int, request, (PublishRequest * publishRequest), (override));
    MOCK_METHOD(int, clientConnect, (), (override));
    MOCK_METHOD(int, clientDisconnect, (), (override));
    MOCK_METHOD(int, subscribeToPrimaryHost, (), (override));
    MOCK_METHOD(int, subscribeToCommands, (), (override));
    MOCK_METHOD(int, unsubscribeToCommands, (), (override));
    MOCK_METHOD(int, publishMessage, (const std::string &topic, uint8_t *buffer, size_t length, DeliveryToken *token), (override));
    MOCK_METHOD(int, configureClient, (ClientOptions * options), (override));

    void connect()
    {
        connected = true;
        SparkplugClient::connected();
    }

    void active()
    {
        SparkplugClient::activated();
    }

    ClientTopicOptions *getTopics()
    {
        return topics;
    }

    int processRequest(PublishRequest *publishRequest)
    {
        return SparkplugClient::processRequest(publishRequest);
    }

    virtual void sync() override
    {
    }

    virtual bool isConnected() override
    {
        return connected;
    }
};

#endif /* TESTS_MOCKS_MOCKSPARKPLUGCLIENT */
