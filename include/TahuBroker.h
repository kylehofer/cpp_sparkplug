/*
 * File: TahuBroker.h
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

#ifndef INCLUDE_TAHUBROKER
#define INCLUDE_TAHUBROKER

#include "SparkplugBroker.h"
#include "MQTTAsync.h"
#include <queue>

using namespace std;

/**
 * @brief 
 * 
 */
class TahuBroker : SparkplugBroker
{
private:
    MQTTAsync client;
    MQTTAsync_connectOptions connectionOptions;
    queue<PublishRequest*> publishQueue;
    std::mutex queueMutex;
    /**
     * @brief 
     * 
     */
    void publishFromQueue();
    /**
     * @brief 
     * 
     */
    void dumpQueue();

protected:
    int clientConnect();
    /**
     * @brief 
     * 
     * @return int 
     */
    int clientDisconnect();
    /**
     * @brief 
     * 
     * @return int 
     */
    int subscribeToPrimaryHost();
    /**
     * @brief 
     * 
     * @return int 
     */
    int subscribeToCommands();
    /**
     * @brief 
     * 
     * @return int 
     */
    int unsubscribeToCommands();
    /**
     * @brief 
     * 
     * @param topic 
     * @param buffer 
     * @param length 
     * @param token 
     * @return int 
     */
    int publishMessage(const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token);
    /**
     * @brief 
     * 
     * @param options 
     * @return int 
     */
    int configureClient(BrokerOptions* options);
    /**
     * @brief Set the Primary object
     * 
     * @param primary 
     * @return true 
     * @return false 
     */
    bool setPrimary(bool primary);
    using SparkplugBroker::getHandler;
    using SparkplugBroker::setState;
    using SparkplugBroker::getState;
    using SparkplugBroker::getPrimary;
public:
    /**
     * @brief Construct a new Tahu Broker object
     * 
     */
    TahuBroker();
    /**
     * @brief Construct a new Tahu Broker object
     * 
     * @param handler 
     * @param options 
     */
    TahuBroker(BrokerEventHandler *handler, BrokerOptions* options);
    /**
     * @brief Destroy the Tahu Broker object
     * 
     */
    ~TahuBroker();
    /**
     * @brief 
     * 
     * @param token 
     */
    void onDelivery(DeliveryToken token);
    /**
     * @brief 
     * 
     * @param token 
     */
    void onDeliveryFailure(DeliveryToken token);
    /**
     * @brief 
     * 
     */
    void onConnect();
    /**
     * @brief 
     * 
     * @param responseCode 
     */
    void onConnectFailure(int responseCode);
    /**
     * @brief 
     * 
     * @param cause 
     */
    void onDisconnect(char *cause);
    /**
     * @brief 
     * 
     * @param topicName 
     * @param topicLen 
     * @param message 
     * @return int 
     */
    int onMessage(char *topicName, int topicLen, MQTTAsync_message *message);
    /**
     * @brief 
     * 
     * @param publishRequest 
     * @return int 
     */
    int requestPublish(PublishRequest* publishRequest);
};

#endif /* INCLUDE_TAHUBROKER */
