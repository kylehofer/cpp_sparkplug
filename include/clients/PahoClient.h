/*
 * File: PahoClient.h
 * Project: sparkplug_c
 * Created Date: Sunday December 18th 2022
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

#ifndef INCLUDE_PAHOCLIENT
#define INCLUDE_PAHOCLIENT

#include "clients/SparkplugClient.h"
#include "CommonTypes.h"
#include <queue>

class PahoClient : public SparkplugClient
{
private:
protected:
    queue<PublishRequest *> publishQueue;
    /**
     * @brief Used to initiate a publish from the PublishRequest queue. If the queue is empty the Client will be set to a Connected State.
     * If there are requests in the queue then the front item of the queue will be published and the Client will be set to a Publishing State.
     */
    void publishFromQueue();
    /**
     * @brief Dumps the publish queue freeing up used memory
     */
    void dumpQueue();
    /**
     * @brief Sets the SparkplugClient as the Primary Client. If set to false the PublishRequest queue will be dumped.
     *
     * @param primary
     */
    void setPrimary(bool primary);

public:
    /**
     * @brief Construct a new Paho MQTT Client
     * This class contains common functionality for both an Async and Synced Paho MQTT Client.
     */
    PahoClient(): SparkplugClient() {}
    /**
     * @brief Construct a new Paho MQTT Client
     *
     * @param handler The Event Handler that manages the callbacks from the Client
     * @param options The options for configuring the MQTT Client
     */
    PahoClient(ClientEventHandler *handler, ClientOptions *options) : SparkplugClient(handler, options) {}
    virtual ~PahoClient();
    /**
     * @brief Callback method for when a publish failed delivery on the client.
     * The Client will re-attempt to send the SparkplugRequest a few times before dumping the request.
     * Finally more requests will be published if the queue contains items
     *
     * @param token The unique token of the failed request
     */
    void onDeliveryFailure(DeliveryToken token);
    /**
     * @brief Callback method when the Client disconnects from a MQTT Host. Will inform the EventHandler that the Client has Disconnected.
     * Marks the Client as Disconnected
     * @param cause The cause of the disconnection
     */
    void onDisconnect(char *cause);
    /**
     * @brief Handles a request to publish data to the MQTT Host
     * If requests are being published then the requests is added to the queue
     *
     * @param publishRequest
     * @return int
     */
    int request(PublishRequest *publishRequest);
};

#endif /* INCLUDE_PAHOCLIENT */
