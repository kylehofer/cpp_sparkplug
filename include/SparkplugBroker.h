/*
 * File: SparkplugBroker.h
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

#ifndef INCLUDE_SPARKPLUGBROKER
#define INCLUDE_SPARKPLUGBROKER

#include <tahu.h>
#include <mutex>
#include "Publisher.h"


#define MAX_TOPIC_LENGTH 256
#define MAX_BUFFER_LENGTH 512
#define PUBLISH_RETRIES 5

typedef int DeliveryToken;

class SparkplugBroker;

typedef struct {
    int payloadlen;
    void* payload;
} SparkplugMessage;


typedef struct {
    bool isBirth;
    Publisher* publisher;
    char* topic;
    DeliveryToken token;
    int retryCount;
} PublishRequest;

class BrokerEventHandler
{
private:
protected:
public:
    virtual void onDelivery(SparkplugBroker *broker, PublishRequest* request) = 0;
    virtual int onMessage(SparkplugBroker *broker, const char *topicName, int topicLen, SparkplugMessage *message) = 0;
    virtual void onActive(SparkplugBroker *broker) = 0;
    virtual void onDeactive(SparkplugBroker *broker) = 0;
    virtual void onConnect(SparkplugBroker *broker) = 0;
    virtual void onDisconnect(SparkplugBroker *broker, char *cause) = 0;
};

typedef int DeliveryToken;

typedef struct
{
    const char* address;
    const char* clientId;
    const char* username;
    const char* password;
    int connectTimeout;
    int keepAliveInterval;
    int retryInterval;
    long timeout;
} BrokerOptions;

typedef struct
{
    char* nodeCommandTopic;
    char* deviceCommandTopic;
    char* primaryHostTopic;
} BrokerTopicOptions;

/*
    PublishRequest_t* publishHead;
    PublishRequest_t* publishTail;
        mutex queueLock;
*/

enum BrokerState
{
    CONNECTED, CONNECTING, DISCONNECTED, DISCONNECTING, PUBLISHING_PAYLOAD
};


class SparkplugBroker
{
private:
    BrokerOptions* options;
    bool configured;
    std::mutex *stateMutex;
    BrokerState state;
    bool isPrimary;
    int encodePayload(org_eclipse_tahu_protobuf_Payload* payload, uint8_t** buffer);
protected:
    BrokerTopicOptions* topics;
    BrokerEventHandler* handler;
    std::mutex publishMutex;
    BrokerEventHandler* getHandler();
    int publishPayload(const char* topic, org_eclipse_tahu_protobuf_Payload* payload, DeliveryToken* token);
    int publish(PublishRequest* publishRequest);
    virtual int clientConnect() = 0;
    virtual int clientDisconnect() = 0;
    virtual int subscribeToPrimaryHost() = 0;
    virtual int subscribeToCommands() = 0;
    virtual int unsubscribeToCommands() = 0;
    virtual int publishMessage(const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token) = 0;
    virtual int configureClient(BrokerOptions* options) = 0;
    void activated();
    void connected();
    void disconnected(char* cause);
    void setState(BrokerState state);
    void setPrimary(bool isPrimary);
    bool getPrimary();

public:
    SparkplugBroker();
    SparkplugBroker(BrokerEventHandler *handler, BrokerOptions* options);
    virtual ~SparkplugBroker();
    int configure(BrokerTopicOptions* topics);
    int activate();
    int deactivate();
    int connect();
    int disconnect();
    virtual int requestPublish(PublishRequest* publishRequest) = 0;
    BrokerState getState();
    static void destroyRequest(PublishRequest* publishRequest);
};

#endif /* INCLUDE_SPARKPLUGBROKER */
