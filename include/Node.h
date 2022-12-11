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

#include "CommonTypes.h"
#include "Publishable.h"
#include "Publisher.h"
#include "Device.h"
#include "SparkplugClient.h"
#include <tahu.h>
#include <vector>
#include <queue>
#include <mutex>
#include <forward_list>
#include "Metrics/CallbackMetric.h"

using namespace std;

/**
 * @brief Base Node Options initializer with Default values
 */

#define NODE_CONTROL_NONE 0
#define NODE_CONTROL_REBIRTH 0b1
#define NODE_CONTROL_NEXT_SERVER 0b10
#define NODE_CONTROL_REBOOT 0b100

#define NodeOptionsInitializer                  \
    {                                           \
        NULL, NULL, NULL, 30, NODE_CONTROL_NONE \
    }

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
    SINGLE,
    PRIMARY_HOST
};

/**
 * @brief Configuration options for a Sparkplug Mode
 * groupId The Sparkplug Group ID
 * nodeId The Sparkplug Node ID
 * primaryHost Optional Primary Host.
 */
typedef struct
{
    const char *groupId;
    const char *nodeId;
    const char *primaryHost;
    int publishPeriod;
    int enabledCommands;
} NodeOptions;

/**
 * @brief A Class representation of a Sparkplug Node.
 * Behaves as a Publishable for publishing Sparkplug Metrics.
 * Manages a set of Clients that are used for Publishing.
 * Can run Asynchronous with Asynchronous supported clients.
 */
class Node : Publishable, ClientEventHandler, Publisher
{
private:
    char *nodeBaseTopic;
    char *deviceBaseTopic;
    ClientTopicOptions clientTopics;
    bool topicsConfigured = false;
    bool running = false;
    bool enabled = false;
    SparkplugClient *activeClient = NULL;
    SparkplugClientMode hostMode;
    vector<SparkplugClient *> clients;
    mutex *asyncLock = new mutex();
    forward_list<Device *> devices;

    // Special Command Metrics
    CallbackMetric *nodeBirthMetric = NULL;

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
    void configureTopics(const char *groupId, const char *nodeId, const char *primaryHost);
    /**
     * @brief Prepares a publish request for a publishable and sends it to the active client
     *
     * @param publishable The publishable to use for the publish request
     * @param isBirth If the message is a birth message
     * @return returns 0 if the request was sent to the client successfully
     */
    int publish(Publishable *publishable, bool isBirth = false);
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
    SparkplugClient *getActiveClient();
    /**
     * @brief Set the Active Client
     *
     * @param activeClient
     */
    void setActiveClient(SparkplugClient *activeClient);
    /**
     * @brief Request a client to activate
     * This will request the client to subscribe to the command topics and
     * the client will be active when the subscriptions are successful
     *
     * @param client
     */
    void activateClient(SparkplugClient *client);
    /**
     * @brief Request a client to deactive.
     * This will request the client to unsubscribe to the command topics and
     * the client will be removed as the active client
     *
     * @param client
     */
    void deactivateClient(SparkplugClient *client);
    /**
     * @brief Add a new client to the Node
     *
     * @param client
     * @return SparkplugClient*
     */
    SparkplugClient *addClient(SparkplugClient *client);
    /**
     * @brief Request all clients to connect to a MQTT Server.
     * Will be called every execute to assure all clients are connected for publishing
     */
    void connect();

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
    template <typename T>
    SparkplugClient *addClient(ClientOptions *options) { return addClient((SparkplugClient *)(new T((ClientEventHandler *)this, options))); };
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
     * all the Publishables (Devices/Node). If any Publishable (Devices/Node) need to be published, then they will have Publish requests send to the Active Client.
     * If no clients are available for publishing all Publishables will wait until they're ready.
     * @param executeTime
     * @return int32_t the minimum time before any Publishable needs to Publish again.
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
    void addDevice(Device *device);
    /**
     * @brief A callback function that is called by Clients when a Publish request has been delivered.
     * This will mark the Publishable as publisher, resetting its publish time.
     *
     * @param client The client responsible for the delivery
     * @param request PublishRequest
     */
    void onDelivery(SparkplugClient *client, PublishRequest *request);
    /**
     * @brief A callback for when a client has received a message on one of its subscribed topics. This should only occur for
     * Node/Device commands, and updates to the Primary host topic (if configured). Commands are passed to the Publishable who owns the Metric.
     * If the Node is configured for a Primary Host then the Active client will change to match the Primary Host.
     *
     * @param client The client responsible for the message
     * @param topicName The topic name of the message
     * @param topicLen The length of the topic string
     * @param message The SparkplugMessage struct containing the raw data
     * @return int
     */
    int onMessage(SparkplugClient *client, const char *topicName, int topicLen, SparkplugMessage *message);
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

    /**
     * @brief Stops the node
     *
     */
    void stop();

    /**
     * @brief A method for when a Publishable requests to be publised.
     * Useful for when a Publishable receives a rebirth command, or is publishing on change of value.
     *
     * @param publishable The publishable making the request
     * @param isBirth Whether the publish should be a birth
     * @return int
     */
    int requestPublish(Publishable *publishable, bool isBirth = false);

    void onMetricCommand(CallbackMetric *metric, SparkplugMessage *message);

    using Publishable::addMetric;
    using Publishable::canPublish;
    using Publishable::getName;
    using Publishable::getPayload;
    using Publishable::update;
};

#endif /* INCLUDE_NODE */
