/*
 * File: SparkplugClient.h
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

#ifndef INCLUDE_SPARKPLUGCLIENT
#define INCLUDE_SPARKPLUGCLIENT

#include <tahu.h>
#include "CommonTypes.h"
#include "Publishable.h"

#define MAX_TOPIC_LENGTH 256
#define MAX_BUFFER_LENGTH 512
#define PUBLISH_RETRIES 5

enum EventType
{
    CLIENT_MESSAGE,
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED,
    CLIENT_ACTIVE,
    CLIENT_DEACTIVE,
    CLIENT_DELIVERED,
    CLIENT_UNDELIVERED
};

typedef struct
{
    const char *topicName;
    int topicLength;
    void *payload;
    int payloadLength;
} MessageEventStruct;

class SparkplugClient;

/**
 * @brief Abstract Class containing the required methods for handling callbacks from a SparkplugClient
 */
class ClientEventHandler
{
private:
protected:
public:
    virtual void onEvent(SparkplugClient *client, EventType eventType, void *data) = 0;
};

typedef int DeliveryToken;

/**
 * @brief Struct containing the configuration options for SparkplugClients
 */
typedef struct
{
    const char *address;
    const char *clientId;
    const char *username;
    const char *password;
    int connectTimeout;
    int keepAliveInterval;
} ClientOptions;

/**
 * @brief Struct containing the subscription topics for SparkplugClients
 */
typedef struct
{
    char *nodeCommandTopic;
    char *nodeDeathTopic;
    char *deviceCommandTopic;
    char *primaryHostTopic;
} ClientTopicOptions;

/**
 * @brief Result codes returned when the Node is enabled
 */
enum ClientState
{
    CONNECTED,
    CONNECTING,
    DISCONNECTED,
    DISCONNECTING,
    PUBLISHING_PAYLOAD
};

/**
 * @brief Abstract Class for handling Sparkplug communictions with an MQTT Host. Handles the state management and commands to and from a Sparkplug Node.
 * Custom clients should use this Class as a base and implement specific methods for their MQTT Client implementations. Supports both asynchronous and
 * synchronous MQTT Clients.
 */
class SparkplugClient
{
private:
    ClientOptions *options = NULL;
    bool configured = false;
    ClientState state = DISCONNECTED;
    bool isPrimary = false;
    ClientEventHandler *handler = NULL;
    /**
     * @brief Encodes a Sparkplug protobuf payload into a raw byte buffer.
     *
     * @param payload The Sparkplug protobuf payload to encode
     * @param buffer A pointer to a buffer array that will contain the payload
     * @return 0 if the encoding was a success
     */
    int encodePayload(org_eclipse_tahu_protobuf_Payload *payload, uint8_t **buffer);

protected:
    ClientTopicOptions *topics;

    /**
     * @brief Get the ClientEventHandler
     *
     * @return ClientEventHandler*
     */
    ClientEventHandler *getHandler();
    /**
     * @brief A function that handles publishing a PublishRequest to an MQTT Client. It will request a payload from a Publishable
     * and encode the payload into a raw buffer. This buffer will then be sent by an MQTT client to a MQTT Host.
     *
     * @param publishRequest PublishRequest to be published
     * @return 0 if the message is both succesfully encoded and sent by the client
     */
    int processRequest(PublishRequest *publishRequest);
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
    virtual int publishMessage(const char *topic, uint8_t *buffer, size_t length, DeliveryToken *token) = 0;
    /**
     * @brief Configures an MQTT Client that will be used for publishing and subscribing to an MQTT host.
     *
     * @param options ClientOptions used to configure the client
     * @return 0 if the client was configured successfully
     */
    virtual int configureClient(ClientOptions *options) = 0;
    /**
     * @brief Callback for when the Client has officially activated. This should occur after the subscription to the Primary
     * Host topic. This will fire the onActive callback of the EventHandler.
     */
    void activated();
    /**
     * @brief Used to mark the SparkplugClient as activated.
     * This will fire the callback on the EventHandler to inform it a client has activated.
     */
    void connected();
    /**
     * @brief Used to mark the SparkplugClient as deactivated.
     * This will fire the callback on the EventHandler to inform it a client has deactivated.
     *
     * @param cause The cause of the disconnection
     */
    void disconnected(char *cause);
    /**
     * @brief Updates the state of the SparkplugClient
     *
     * @param state
     */
    void delivered(PublishRequest *publishRequest);
    void undelivered(PublishRequest *publishRequest);
    void messageReceived(const char *topicName, int topicLength, void *payload, int payloadLength);
    void setState(ClientState state);
    /**
     * @brief Sets the SparkplugClient as the Primary Client
     *
     * @param isPrimary
     */
    void setPrimary(bool isPrimary);
    /**
     * @brief Gets whether the SparkplugClient is the Primary Client
     *
     * @return true
     * @return false
     */
    bool getPrimary();

public:
    /**
     * @brief Construct a new SparkplugClient
     *
     */
    SparkplugClient();
    /**
     * @brief Construct a new SparkplugClient
     *
     * @param handler The Event Handler that manages the callbacks from the Client
     * @param options The options for configuring the MQTT Client
     */
    SparkplugClient(ClientEventHandler *handler, ClientOptions *options);
    virtual ~SparkplugClient();
    /**
     * @brief Configures the SparkplugClient by setting up a MQTT Client and setting the required
     * Sparkplug topics.
     *
     * @param topics Contains the Device/Node topics for commands. Contains the Primary host topic if configured.
     * @return 0 if the topics were configured successfully
     */
    int configure(ClientTopicOptions *topics);
    /**
     * @brief Request a client to activate
     * This will request the client to subscribe to the command topics and
     * the client will be active when the subscriptions are successful
     *
     * @return 0 if the request was sent successfully
     */
    int activate();
    /**
     * @brief Request a client to deactive.
     * This will request the client to unsubscribe to the command topics and
     * the client will be removed as the active client
     *
     * @return 0 if the request was sent successfully
     */
    int deactivate();
    /**
     * @brief Requests the SparkplugClient to connect to the MQTT Host.
     *
     * @return 0 if the request was sent successfully
     */
    int connect();
    /**
     * @brief Requests the SparkplugClient to disconnect from the MQTT Host.
     *
     * @return 0 if the request was sent successfully
     */
    int disconnect();
    /**
     * @brief Used to send a PublishRequest to the SparkplugClient. Implementions will handle whether the Client is asynchronous or synchronous.
     * NOTE: It is upto the Client implementation to free the memory used by the PublishRequest.
     * @param publishRequest
     * @return 0 if the request was sent successfully
     */
    virtual int request(PublishRequest *publishRequest) = 0;
    /**
     * @brief Get the current state of the Client
     *
     * @return ClientState
     */
    ClientState getState();
    /**
     * @brief Frees memory used by a PublishRequest.
     *
     * @param publishRequest
     */
    static void destroyRequest(PublishRequest *publishRequest);

    /**
     * @brief Assures the SparkplugClient is connected and synced to the broker.
     * Called by the node on every execute.
     */
    void execute();

    /**
     * @brief Syncs the SparkplugClient assuring all data and callbacks have fired.
     * Used for Synchronous MQTT Clients.
     */
    virtual void sync() = 0;
    /**
     * @brief Returns whether the client is connected
     *
     * @return true
     * @return false
     */
    virtual bool isConnected() = 0;
};

#endif /* INCLUDE_SPARKPLUGCLIENT */
