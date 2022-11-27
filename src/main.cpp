/*
 * File: main.cpp
 * Project: cpp_sparkplug
 * Created Date: Thursday November 17th 2022
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

#include "Node.h"
#include "Device.h"
#include "TahuClient.h"
#include <iostream>

int main(int argc, char *argv[])
{
    cout << "Creating Node\n";

    NodeOptions nodeOptions = {
        "GroupId", "NodeId", NULL, 5
    };

    Node node = Node(&nodeOptions);

    cout << "Creating Client Options\n";
    ClientOptions options = {
        .address = "",
        .clientId = "unique_id",
        .username = NULL,
        .password = NULL,
        .connectTimeout = 60,
        .keepAliveInterval = 5,
        .retryInterval = 5000,
        .timeout = 10000
    };

    node.addClient<TahuClient>(&options);

    Device device = Device("MyDevice", 1);

    Device* devices[] = { &device };

    node.setDevices(devices, 1);


    return 0;
}