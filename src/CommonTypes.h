/*
 * File: CommonTypes.h
 * Project: cpp_sparkplug
 * Created Date: Thursday December 1st 2022
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

#ifndef INCLUDE_COMMONTYPES
#define INCLUDE_COMMONTYPES

#include <string>

class Publishable;

typedef int DeliveryToken;

/**
 * @brief Struct for handling the data required for publishing requests.
 */
typedef struct
{
    bool isBirth;
    Publishable *publisher;
    char *topic;
    DeliveryToken token;
    int retryCount;
} PublishRequest;

class Uri
{
private:
    std::string protocol;
    std::string address;
    int port = -1;

    void init(std::string uri)
    {
        // tcp://192.168.40.250:1884
        const std::string protocolIdentifier("://"), portSeparator(":");

        std::size_t addressStart, portStart;

        addressStart = uri.find(protocolIdentifier);
        if (addressStart == std::string::npos)
        {
            // Error no protocol
            addressStart = 0;
        }
        else
        {
            addressStart += protocolIdentifier.length();
            protocol = uri.substr(0, addressStart);
        }

        portStart = uri.find(portSeparator, addressStart);

        if (portStart == std::string::npos)
        {
            // Error no port
            address = uri.substr(addressStart);
            port = -1;
        }
        else
        {
            portStart += portSeparator.length();
            address = uri.substr(addressStart, portStart - addressStart - 1);
            port = stoi(uri.substr(portStart));
        }
    }

protected:
public:
    Uri(std::string protocol, std::string address, int port) : protocol(protocol), address(address), port(port)
    {
    }

    Uri(std::string uri)
    {
        init(uri);
    }

    Uri(const char *uri)
    {
        std::string input(uri);
        init(input);
    }

    const std::string &getProtocol()
    {
        return protocol;
    }

    const std::string &getAddress()
    {
        return address;
    }

    const int &getPort()
    {
        return port;
    }

    const std::string toString()
    {
        if (port > 0)
        {
            return protocol + address + ":" + std::to_string(port);
        }
        return protocol + address;
    }
};

#endif /* INCLUDE_COMMONTYPES */
