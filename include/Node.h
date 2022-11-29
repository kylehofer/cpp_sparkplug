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
#include "SparkplugClient.h"
#include <tahu.h>
#include <vector>
#include <queue>
#include <mutex>

using namespace std;

/**
 * @brief Base Node Options initializer with Default values
 */
#define NodeOptionsInitializer { NULL, NULL, NULL, 30 }

/**
 * @brief Result codes returned when the Node is enabled
 */
enum SparkplugNodeEnableResult
{
    ENABLE_SUCCESS = 0,
    ENABLE_INVALID_TOPICS = -1,
    ENABLE_NO_CLIENTS = -2,
    ENABLE_CLIENT_CONFIG_FAIL = -3
};

/**
 * @brief The Client Mode the Node will be operating with
 */
enum SparkplugClientMode
{
    SINGLE, PRIMARY_HOST
};

/**
 * @brief Configuration options for a Sparkplug Mode
 * groupId The Sparkplug Group ID
 * nodeId The Sparkplug Node ID
 * primaryHost Optional Primary Host.
 */
typedef struct 
{
    const char* groupId;
    const char* nodeId;
    const char* primaryHost;
    int publishPeriod;
} NodeOptions;

/**
 * @brief A Class representation of a Sparkplug Node.
 * Behaves as a Publisher for publishing Sparkplug Metrics.
 * Manages a set of Clients that are used for Publishing.
 * Can run Asynchronous with Asynchronous supported clients.
 */
class Node : Publisher, ClientEventHandler
{
private:
    char* nodeBaseTopic;
    char* deviceBaseTopic;
    ClientTopicOptions clientTopics;
    bool topicsConfigured;
    bool running;
    bool enabled;
    Device** devices;
    int8_t deviceCount;
    SparkplugClient* activeClient;
    SparkplugClientMode hostMode;
    vector<SparkplugClient*> clients;
    mutex *asyncLock;
    
    /**
     * @brief Publish a Birth message the node and all devices
     */
    void publishBirth();
    /**
     * @brief Configures the topics required for Sparkplug.
     * These topics will be used for all clients configured for the node
     * 
     * @param groupId The Sparkplug Group ID
     * @param nodeId The Sparkplug Node ID
     * @param primaryHost Optional Primary Host.
     */
    void configureTopics(const char* groupId, const char* nodeId, const char *primaryHost);
    /**
     * @brief Prepares a publish request for the node and sends it to the active client
     * 
     * @param isBirth If the message is a birth message
     * @return returns 0 if the request was sent to the client successfully
     */
    int publish(bool isBirth);
    /**
     * @brief Prepares a publish request for a device and sends it to the active client
     * 
     * @param device The device to use for the publish request
     * @param isBirth If the message is a birth message
     * @return returns 0 if the request was sent to the client successfully
     */
    int publish(Device* device, bool isBirth);
    /**
     * @brief Set the Client Mode
     * 
     * @param mode 
     */
    void setClientMode(SparkplugClientMode mode);
    /**
     * @brief Get the current Client Mode
     * 
     * @return SparkplugClientMode 
     */
    SparkplugClientMode getClientMode();
    /**
     * @brief Get the Active Client
     * 
     * @return SparkplugClient* 
     */
    SparkplugClient* getActiveClient();
    /**
     * @brief Set the Active Client
     * 
     * @param activeClient 
     */
    void setActiveClient(SparkplugClient* activeClient);
    /**
     * @brief Request a client to activate
     * This will request the client to subscribe to the command topics and
     * the client will be active when the subscriptions are successful
     * 
     * @param client 
     */
    void activateClient(SparkplugClient* client);
    /**
     * @brief Request a client to deactive.
     * This will request the client to unsubscribe to the command topics and
     * the client will be removed as the active client
     * 
     * @param client 
     */
    void deactivateClient(SparkplugClient* client);
    /**
     * @brief Add a new client to the Node
     * 
     * @param client 
     * @return SparkplugClient* 
     */
    SparkplugClient* addClient(SparkplugClient *client);
    /**
     * @brief Request all clients to connect to a MQTT Server.
     * Will be called every execute to assure all clients are connected for publishing
     */
    void connect();
protected:
    /**
     * @brief Initializes a base payload for adding metrics to.
     * Will add Command metrics if it is a Birth payload
     * NOTE: Memory is allocated for the payload, and must be freed by the caller of this function.
     * 
     * @param isBirth 
     * @return org_eclipse_tahu_protobuf_Payload* 
     */
    org_eclipse_tahu_protobuf_Payload* initializePayload(bool isBirth);

public:
    /**
     * @brief Construct a new Sparkplug Node
     * 
     */
    Node();
    /**
     * @brief Construct a new Sparkplug Node
     * 
     * @param options Configuration options for the Node
     */
    Node(NodeOptions *options);
    ~Node();
    /**
     * @brief Add a new client to the Node
     * 
     * @tparam T The base Class that will be added. Must extend SparkplugClient
     * @param options Configuration options for the SparkplugClient
     * @return SparkplugClient* 
     */
    template <typename T> SparkplugClient* addClient(ClientOptions *options) { return addClient((SparkplugClient*) (new T((ClientEventHandler*) this, options))); };
    /**
     * @brief This function enables the node so it can begin to publish Sparkplug Payloads.
     * It will ensure the required topics have been configured, Clients have been added to the node, 
     * and will request all Clients configure their clients. Must be called before any Sparkplug communications can occur.
     * 
     * @return SparkplugNodeEnableResult.ENABLE_SUCCESS if the Node was successfully enabled.
     */
    int enable();
    /**
     * @brief TODO: Better Name
     * 
     */
    void begin();
    /**
     * @brief The main execute function of the Node. It will ensure all clients are connected. It will update the publish time left for 
     * all the Publishers (Devices/Node). If any Publisher (Devices/Node) need to be published, then they will have Publish requests send to the Active Client.
     * If no clients are available for publishing all Publishers will wait until they're ready.
     * @param executeTime 
     * @return int32_t the minimum time before any Publisher needs to Publish again.
     */
    int32_t execute(int32_t executeTime);
    /**
     * @brief TODO: Implement. It should switch to the next client in the list.
     * 
     * @return int 
     */
    int nextServer();
    /**
     * @brief Sets the Devices on the Node.
     * 
     * @param devices An array of Device pointers
     * @param deviceCount The number of Devices being added
     */
    void setDevices(Device** devices, int8_t deviceCount);
    /**
     * @brief A callback function that is called by Clients when a Publish request has been delivered.
     * This will mark the Publisher as publisher, resetting its publish time.
     * 
     * @param client The client responsible for the delivery
     * @param request PublishRequest
     */
    void onDelivery(SparkplugClient *client, PublishRequest* request);
    /**
     * @brief A callback for when a client has received a message on one of its subscribed topics. This should only occur for
     * Node/Device commands, and updates to the Primary host topic (if configured). Commands are passed to the Publisher who owns the Metric.
     * If the Node is configured for a Primary Host then the Active client will change to match the Primary Host.
     * 
     * @param client The client responsible for the message
     * @param topicName The topic name of the message
     * @param topicLen The length of the topic string
     * @param message The SparkplugMessage struct containing the raw data
     * @return int 
     */
    int onMessage(SparkplugClient *client, const char *topicName, int topicLen, SparkplugMessage* message);
    /**
     * @brief A callback when a client has been succesfully activated. This client will then be used as the activate client and 
     * messages will start publishing from this client.
     * 
     * @param client 
     */
    void onActive(SparkplugClient *client);
    /**
     * @brief A callback when a client has been succesfully deactivated.
     * 
     * @param client The client responsible for the deactivation
     */
    void onDeactive(SparkplugClient *client);
    /**
     * @brief A callback when a client has succesfully connected to a MQTT Host. If the node is configured without a primary host the client
     * will be activated as the primary client.
     * 
     * @param client The client responsible for the connection
     */
    void onConnect(SparkplugClient *client);
    /**
     * @brief A callback when a client has disconnected from a MQTT Host.
     * 
     * @param client The client responsible for the disconnection
     * @param cause The cause of the disconnection
     */
    void onDisconnect(SparkplugClient *client, char *cause);
    /**
     * @brief Returns whether the node has an activate broker it can publish with
     * 
     * @return true 
     * @return false 
     */
    bool isActive();
    using Publisher::update;
    using Publisher::getPayload;
    using Publisher::setMetrics;

    void stop();
};

#endif /* INCLUDE_NODE */
