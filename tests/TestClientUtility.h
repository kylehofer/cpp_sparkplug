/*
 * File: TestClientUtility.cpp
 * Project: sparkplug_c
 * Created Date: Monday November 28th 2022
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

#ifndef TESTS_TESTCLIENTUTILITY
#define TESTS_TESTCLIENTUTILITY

#include <queue>
#include <cstddef>

using namespace std;

typedef struct {
    char *topic;
    void *payload;
    int payloadlen;
   
} PayloadMessage;


/**
 * @brief A testing utility class for getting and sending messages to and from a broker
 */
class TestClientUtility
{
private:
    struct mosquitto *client;
    queue<PayloadMessage> message;
    bool ready;
    queue<PayloadMessage> messageQueue;
protected:
public:
    /**
     * @brief Construct a new Test Client Utility object
     * 
     */
    TestClientUtility() : client(NULL), ready(false) {};
    ~TestClientUtility();
    /**
     * @brief Initialized a mosquitto client and connect to a Broker
     * 
     * @param host The host address
     * @param port The broker port
     * @return Non-zero return if something failed
     */
    int connect(const char *host, int port);
    /**
     * @brief Sets the test client as ready for receiving messages
     * 
     * @param ready 
     */
    void setReady(bool ready);
    /**
     * @brief Returns whether the test client is ready for receiving messages
     * Calls a non blocking mosquitto_loop
     * 
     * @return true 
     * @return false 
     */
    bool isReady();
    /**
     * @brief A blocking call that will wait for the test client to be ready
     * for receiving messages.
     * 
     * @param maxRetries The number of checks before failing 
     * @param timePerLoop Number of milliseconds between each check
     * @return true 
     * @return false 
     */
    bool waitForReady(int maxRetries, int timePerLoop = 5);
    /**
     * @brief Returns whether the test client has data available
     * Calls a non blocking mosquitto_loop
     * 
     * @return true 
     * @return false 
     */
    bool hasData();
    /**
     * @brief A blocking call that will wait for data to be receieved by the test client.
     * 
     * @param maxRetries The number of attempts to check for data
     * @param timePerLoop Number of milliseconds between each check
     * @return true 
     * @return false 
     */
    void waitForData(int maxRetries, int timePerLoop = 5);
    /**
     * @brief Returns the front of the data queue.
     * 
     * @return PayloadMessage 
     */
    PayloadMessage pop();
    /**
     * @brief Pushes data to the front of the queue.
     * 
     * @param message 
     */
    void push(PayloadMessage message);
    int publish(const char *topic, void *data, size_t dataLength, bool retain = false);
    /**
     * @brief 
     * 
     */
    static void cleanup();
    /**
     * @brief Frees memory used by a PayloadMessage
     * 
     * @param payload 
     */
    static void freePayload(PayloadMessage payload);
};


#endif /* TESTS_TESTCLIENTUTILITY */
