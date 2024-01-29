/*
 * File: TestTcpClient.h
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

#ifndef TESTS_UTILS_TESTTCPCLIENT
#define TESTS_UTILS_TESTTCPCLIENT

#include "stdint.h"
#include "Client.h"

using namespace std;

typedef struct DataBuffer_t
{
    const uint8_t *data;
    const uint8_t *position;
    size_t length;
    DataBuffer_t *next;
} DataBuffer;

/**
 * @brief A test utility class for forwarding data through a socket.
 * Allows the control of data to simulate lost data or disconnections
 */
class TestTcpClient : Client
{
private:
    int port;
    int openSocket;
    bool stopped = true;
    bool blocked = false;

    int availableData = 0;
    size_t writeSize = 0;

    DataBuffer *receiveQueue = NULL;
    DataBuffer *writeQueue = NULL;

    /**
     * @brief closes a socket
     *
     * @param port
     * @return int
     */
    static void closeSocket(int socket);

    int open(int port);

public:
    TestTcpClient() : port(0){};
    TestTcpClient(int port) : port(port){};
    ~TestTcpClient();

    void flush();

    virtual int connect(const char *host, uint16_t port) override;
    virtual size_t write(uint8_t) override;
    virtual size_t write(const void *buffer, size_t size) override;
    virtual int available() override;
    virtual int read(void *buffer, size_t size) override;
    virtual void stop() override;
    virtual uint8_t connected() override;

    void sync();

    /**
     * @brief Starts port forwarding
     *
     * @return int returns non 0 if it failed to setup forwarding
     */
    int start();

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

#endif /* TESTS_UTILS_TESTTCPCLIENT */
