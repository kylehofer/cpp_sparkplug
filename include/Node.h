/*
 * File: Node.h
 * Project: cpp_sparkplug
 * Created Date: Sunday September 25th 2022
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

#ifndef INCLUDE_NODE
#define INCLUDE_NODE

#include "Publisher.h"
#include "Device.h"
#include "SparkplugBroker.h"
#include <tahu.h>
#include <vector>
#include <queue>
#include <mutex>

using namespace std;

#define NodeOptionsInitializer { NULL, NULL, NULL, 30 }

enum SparkplugNodeEnableResult
{
    ENABLE_SUCCESS = 0,
    ENABLE_INVALID_TOPICS = -1,
    ENABLE_NO_BROKERS = -2,
    ENABLE_BROKER_CONFIG_FAIL = -3
};

enum SparkplugBrokerMode
{
    SINGLE, PRIMARY_HOST
};

typedef struct 
{
    const char* groupId;
    const char* nodeId;
    const char* primaryHost;
    int publishPeriod;
} NodeOptions;

class Node : Publisher, BrokerEventHandler
{
private:
    char* nodeBaseTopic;
    char* deviceBaseTopic;
    BrokerTopicOptions brokerTopics;
    bool topicsConfigured;
    bool running;
    bool enabled;
    Device** devices;
    int8_t deviceCount;
    SparkplugBroker* activeBroker;
    SparkplugBrokerMode hostMode;
    vector<SparkplugBroker*> brokers;
    mutex *asyncLock;
    
    void publishBirth();
    void configureTopics(const char* groupId, const char* nodeId, const char *primaryHost);
    int publish(bool isBirth);
    int publish(Device* device, bool isBirth);
    void setBrokerMode(SparkplugBrokerMode mode);
    SparkplugBrokerMode getBrokerMode();
    SparkplugBroker* getActiveBroker();
    void setActiveBroker(SparkplugBroker* activeBroker);
    void activateBroker(SparkplugBroker* broker);
    void deactivateBroker(SparkplugBroker* broker);
    SparkplugBroker* addBroker(SparkplugBroker *broker);
    void connect();
protected:
    org_eclipse_tahu_protobuf_Payload* initializePayload(bool isBirth);
    

public:
    Node();
    Node(NodeOptions *options);
    ~Node();
    void start();
    void stop();
    template <typename T> SparkplugBroker* addBroker(BrokerOptions *options) { return addBroker((SparkplugBroker*) (new T((BrokerEventHandler*) this, options))); };
    int enable();
    void begin();
    int32_t execute(int32_t executeTime);
    int nextServer();
    void setDevices(Device** devices, int8_t deviceCount);
    void onDelivery(SparkplugBroker *broker, PublishRequest* request);
    int onMessage(SparkplugBroker *broker, const char *topicName, int topicLen, SparkplugMessage* message);
    void onActive(SparkplugBroker *broker);
    void onDeactive(SparkplugBroker *broker);
    void onConnect(SparkplugBroker *broker);
    void onDisconnect(SparkplugBroker *broker, char *cause);
    using Publisher::update;
    using Publisher::getPayload;

};

#endif /* INCLUDE_NODE */
