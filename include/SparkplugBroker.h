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

/**
 * @brief Struct for storing Sparkplug messages.
 * Contains a pointer to the raw data along with the size of the data
 */
typedef struct {
    int payloadlen;
    void* payload;
} SparkplugMessage;

/**
 * @brief Struct for handling the data required for publishing requests.
 */
typedef struct {
    bool isBirth;
    Publisher* publisher;
    char* topic;
    DeliveryToken token;
    int retryCount;
} PublishRequest;

/**
 * @brief Abstract Class containing the required methods for handling callbacks from a SparkplugBroker
 */
class BrokerEventHandler
{
private:
protected:
public:
    /**
     * @brief Callback method for when a SparkplugRequest has been delivered on a broker.
     * 
     * @param broker The broker responsible for the callback
     * @param request PublishRequest
     */
    virtual void onDelivery(SparkplugBroker *broker, PublishRequest* request) = 0;
    /**
     * @brief 
     * 
     * @param broker The broker responsible for the callback
     * @param topicName The topic name of the message
     * @param topicLen The length of the topic string
     * @param message The SparkplugMessage struct containing the raw data
     * @return int 
     */
    virtual int onMessage(SparkplugBroker *broker, const char *topicName, int topicLen, SparkplugMessage *message) = 0;
    /**
     * @brief A callback when a broker has been succesfully activated. An active broker is when it has subscribed to required command topics.
     * 
     * @param broker The broker responsible for the callback
     */
    virtual void onActive(SparkplugBroker *broker) = 0;
    /**
     * @brief A callback when a broker has been succesfully deactivated.
     * 
     * @param broker The broker responsible for the callback
     */
    virtual void onDeactive(SparkplugBroker *broker) = 0;
    /**
     * @brief A callback when a broker has succesfully connected to a MQTT Host.
     * 
     * @param broker The broker responsible for the callback
     */
    virtual void onConnect(SparkplugBroker *broker) = 0;
    /**
     * @brief A callback when a broker has disconnected to a MQTT Host.
     * 
     * @param broker The broker responsible for the callback
     * @param cause 
     */
    virtual void onDisconnect(SparkplugBroker *broker, char *cause) = 0;
};

typedef int DeliveryToken;

/**
 * @brief Struct containing the configuration options for SparkplugBrokers
 */
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

/**
 * @brief Struct containing the subscription topics for SparkplugBrokers
 */
typedef struct
{
    char* nodeCommandTopic;
    char* deviceCommandTopic;
    char* primaryHostTopic;
} BrokerTopicOptions;

/**
 * @brief Result codes returned when the Node is enabled
 */
enum BrokerState
{
    CONNECTED, CONNECTING, DISCONNECTED, DISCONNECTING, PUBLISHING_PAYLOAD
};

/**
 * @brief Abstract Class for handling Sparkplug communictions with an MQTT Host. Handles the state management and commands to and from a Sparkplug Node.
 * Custom brokers should use this Class as a base and implement specific methods for their MQTT Client implementations. Supports both asynchronous and
 * synchronous MQTT Clients.
 */
class SparkplugBroker
{
private:
    BrokerOptions* options;
    bool configured;
    std::mutex *stateMutex;
    BrokerState state;
    bool isPrimary;
    /**
     * @brief Encodes a Sparkplug protobuf payload into a raw byte buffer.
     * 
     * @param payload The Sparkplug protobuf payload to encode
     * @param buffer A pointer to a buffer array that will contain the payload
     * @return 0 if the encoding was a success
     */
    int encodePayload(org_eclipse_tahu_protobuf_Payload* payload, uint8_t** buffer);
protected:
    BrokerTopicOptions* topics;
    BrokerEventHandler* handler;
    std::mutex publishMutex;
    /**
     * @brief Get the BrokerEventHandler
     * 
     * @return BrokerEventHandler* 
     */
    BrokerEventHandler* getHandler();
    /**
     * @brief A function that handles publishing a PublishRequest to an MQTT Broker. It will request a payload from a Publisher
     * and encode the payload into a raw buffer. This buffer will then be sent by an MQTT client to a MQTT Host.
     * 
     * @param publishRequest PublishRequest to be published
     * @return 0 if the message is both succesfully encoded and sent by the client
     */
    int publish(PublishRequest* publishRequest);
    /**
     * @brief Requests the SparkplugClient to connect to the MQTT Host
     * 
     * @return 0 if the request was sent succesfully
     */
    virtual int clientConnect() = 0;
    /**
     * @brief Requests the SparkplugClient todisconnect from the MQTT Host
     * 
     * @return 0 if the request was sent succesfully
     */
    virtual int clientDisconnect() = 0;
    /**
     * @brief Requests the SparkplugClient to subscribe to the Primary Host topic
     * 
     * @return 0 if the request was sent succesfully
     */
    virtual int subscribeToPrimaryHost() = 0;
    /**
     * @brief Requests the SparkplugClient to subscribe to the command topics
     * 
     * @return 0 if the request was sent succesfully
     */
    virtual int subscribeToCommands() = 0;
    /**
     * @brief Requests the SparkplugClient to unsubscribe from the command topics
     * 
     * @return 0 if the request was sent succesfully
     */
    virtual int unsubscribeToCommands() = 0;
    /**
     * @brief Requests the SparkplugClient to publish a buffer to a MQTT Host
     * 
     * @param topic The topic name to publish the buffer to
     * @param buffer The buffer being published
     * @param length The size of the buffer being published
     * @param token A unique token that will be attached to the message being sent. Used to identify when messages are delivered by asynchronous clients
     * @return 0 if the request was sent succesfully
     */
    virtual int publishMessage(const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token) = 0;
    /**
     * @brief Configures an MQTT Client that will be used for publishing and subscribing to an MQTT host.
     * 
     * @param options BrokerOptions used to configure the client
     * @return 0 if the client was configured successfully
     */
    virtual int configureClient(BrokerOptions* options) = 0;
    /**
     * @brief 
     * 
     */
    void activated();
    /**
     * @brief Used to mark the SparkplugBroker as activated.
     * This will fire the callback on the EventHandler to inform it a broker has activated.
     */
    void connected();
    /**
     * @brief Used to mark the SparkplugBroker as deactivated.
     * This will fire the callback on the EventHandler to inform it a broker has deactivated.
     * 
     * @param cause The cause of the disconnection
     */
    void disconnected(char* cause);
    /**
     * @brief Updates the state of the SparkplugBroker
     * 
     * @param state 
     */
    void setState(BrokerState state);
    /**
     * @brief Sets the SparkplugBroker as the Primary Client
     * 
     * @param isPrimary 
     */
    void setPrimary(bool isPrimary);
    /**
     * @brief Gets whether the SparkplugBroker is the Primary Client
     * 
     * @return true 
     * @return false 
     */
    bool getPrimary();

public:
    /**
     * @brief Construct a new SparkplugBroker
     * 
     */
    SparkplugBroker();
    /**
     * @brief Construct a new SparkplugBroker
     * 
     * @param handler The Event Handler that manages the callbacks from the Client
     * @param options The options for configuring the MQTT Client
     */
    SparkplugBroker(BrokerEventHandler *handler, BrokerOptions* options);
    virtual ~SparkplugBroker();
    /**
     * @brief Configures the SparkplugBroker by setting up a MQTT Client and setting the required
     * Sparkplug topics.
     * 
     * @param topics Contains the Device/Node topics for commands. Contains the Primary host topic if configured.
     * @return 0 if the topics were configured successfully
     */
    int configure(BrokerTopicOptions* topics);
    /**
     * @brief Request a broker to activate
     * This will request the broker to subscribe to the command topics and
     * the broker will be active when the subscriptions are successful
     * 
     * @return 0 if the request was sent successfully 
     */
    int activate();
    /**
     * @brief Request a broker to deactive.
     * This will request the broker to unsubscribe to the command topics and
     * the broker will be removed as the active broker
     * 
     * @return 0 if the request was sent successfully 
     */
    int deactivate();
    /**
     * @brief Requests the SparkplugBroker to connect to the MQTT Host.
     * 
     * @return int 
     */
    int connect();
    /**
     * @brief 
     * 
     * @return int 
     */
    int disconnect();
    /**
     * @brief 
     * 
     * @param publishRequest 
     * @return int 
     */
    virtual int requestPublish(PublishRequest* publishRequest) = 0;
    /**
     * @brief Get the State object
     * 
     * @return BrokerState 
     */
    BrokerState getState();
    /**
     * @brief 
     * 
     * @param publishRequest 
     */
    static void destroyRequest(PublishRequest* publishRequest);
};

#endif /* INCLUDE_SPARKPLUGBROKER */
