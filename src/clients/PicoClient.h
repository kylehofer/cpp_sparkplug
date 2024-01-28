/*
 * File: PicoClient.h
 * Project: cpp_sparkplug
 * Created Date: Monday June 12th 2023
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

#ifndef SRC_CLIENTS_PICOCLIENT
#define SRC_CLIENTS_PICOCLIENT

#include "CppMqttClient.h"
#include "PicoTcpClient.h"

class PicoClient : public CppMqttClient
{
private:
    PicoTcpClient tcpClient;

protected:
    /**
     * @brief Get the Client object
     *
     * @return Client*
     */
    virtual Client *getClient() override
    {
        return (Client *)&tcpClient;
    };

public:
    /**
     * @brief Construct a new Pico MQTT Client
     *
     * @param handler The Event Handler that manages the callbacks from the Client
     * @param options The options for configuring the MQTT Client
     */
    PicoClient(ClientEventHandler *handler, ClientOptions *options) : CppMqttClient(handler, options)
    {
    }
};

#endif /* SRC_CLIENTS_PICOCLIENT */
