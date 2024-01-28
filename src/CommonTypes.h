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

#ifndef SRC_COMMONTYPES
#define SRC_COMMONTYPES

#include <string>

class Publishable;

typedef int DeliveryToken;

/**
 * @brief Struct for handling the data required for publishing requests.
 */
struct PublishRequest
{
    PublishRequest(bool isBirth,
                   Publishable *publisher,
                   std::string topic,
                   DeliveryToken token,
                   int retryCount) : isBirth(isBirth),
                                     publisher(publisher),
                                     topic(topic),
                                     token(token),
                                     retryCount(retryCount){};
    bool isBirth;
    Publishable *publisher;
    std::string topic;
    DeliveryToken token;
    int retryCount;
};

/**
 * @brief Struct for handling URIs
 *
 */
class Uri
{
private:
    std::string protocol;
    std::string address;
    int port = -1;

    /**
     * @brief Initializes a URI from a string
     *
     * @param uri
     */
    void init(std::string uri)
    {
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
    /**
     * @brief Construct a new Uri object from a protocol, address, and port
     *
     * @param protocol
     * @param address
     * @param port
     */
    Uri(std::string protocol, std::string address, int port) : protocol(protocol), address(address), port(port)
    {
    }

    /**
     * @brief Construct a new Uri object from a string
     *
     * @param uri
     */
    Uri(std::string uri)
    {
        init(uri);
    }

    /**
     * @brief Construct a new Uri object from a character string
     *
     * @param uri
     */
    Uri(const char *uri)
    {
        std::string input(uri);
        init(input);
    }

    /**
     * @brief Get the Protocol of the uri
     *
     * @return const std::string&
     */
    const std::string &getProtocol()
    {
        return protocol;
    }

    /**
     * @brief Get the Address of the uri
     *
     * @return const std::string&
     */
    const std::string &getAddress()
    {
        return address;
    }

    /**
     * @brief Get the Port of the uri
     *
     * @return const int&
     */
    const int &getPort()
    {
        return port;
    }

    /**
     * @brief Returns the string representation of the URI
     *
     * @return const std::string
     */
    const std::string toString()
    {
        if (port > 0)
        {
            return protocol + address + ":" + std::to_string(port);
        }
        return protocol + address;
    }
};

#endif /* SRC_COMMONTYPES */
