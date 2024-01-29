/*
 * File: PortForwarder.h
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

#ifndef TESTS_UTILS_PORTFORWARDER
#define TESTS_UTILS_PORTFORWARDER

#include <thread>
#include <vector>
#include <mutex>

using namespace std;

/**
 * @brief A test utility class for forwarding data through a socket.
 * Allows the control of data to simulate lost data or disconnections
 */
class PortForwarder
{
private:
    int inPort, outPort;
    int inSocket, outSocket;
    bool stopped = true;
    bool blocked = false;

    vector<thread *> threads;
    vector<int> sockets;

    mutex *portLock = new mutex();

    /**
     * @brief Opens a listener socket
     *
     * @param port
     * @return int
     */
    static int openListener(int port);

    /**
     * @brief Opens an output socket
     *
     * @param port
     * @return int
     */
    static int openOutput(int port);

    /**
     * @brief closes a socket
     *
     * @param port
     * @return int
     */
    static void closeSocket(int socket);

    /**
     * @brief Main function that will be run on a seperate thread
     * Will monitor for new connections and setup a pair of connections for forwarding the data
     */
    void main();

    /**
     * @brief Sets up a listener that will forward all traffic from the source to the destination
     *
     * @param source
     * @param destination
     */
    void listener(int source, int destination);

    /**
     * @brief Whether the port is blocking.
     * Thread safe.
     *
     * @return true
     * @return false output
     */
    bool isBlocked();

    /**
     * @brief Whether the port forwarding has stopped.
     * Thread safe.
     *
     * @return true
     * @return false
     */
    bool isStopped();

public:
    PortForwarder(PortForwarder &&) = default;
    PortForwarder(int inPort, int outPort) : inPort(inPort), outPort(outPort){};
    ~PortForwarder();

    /**
     * @brief Starts port forwarding
     *
     * @return int returns non 0 if it failed to setup forwarding
     */
    int start();

    /**
     * @brief Stops port forwarding, and closes active threads.
     *output
     */
    void stop();

    /**
     * @brief Causes the port forwarder to start blocking traffic.
     * Thread safe.
     *
     */
    void block();

    /**
     * @brief Causes the port forwarder to stop blocking traffic.
     * Thread safe.
     *
     */
    void unblock();

    /**
     * @brief Utility method for checking if a port is open
     * Blocking call and will retry until the port is open
     *
     * @param port
     * @param maxRetries
     * @return true
     * @return false
     */
    static bool portCheck(int port, int maxRetries = 5);
};

#endif /* TESTS_UTILS_PORTFORWARDER */
