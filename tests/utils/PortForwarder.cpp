#include "utils/PortForwarder.h"

#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>

#include <cerrno>
#include <fcntl.h>

#include <chrono>
#include <thread>

#include <unistd.h>

using namespace std;
using ::chrono::milliseconds;
using ::this_thread::sleep_for;

#define MAX_RETRIES 400
#define RETRY_TIMEOUT 5

#define FORWARDER_BUFFER_SIZE 4096
#define LISTENER_BACKLOG 40

int PortForwarder::openListener(int port)
{
    struct sockaddr_in address;
    int listenerSocket, result, flags;

    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenerSocket < 0)
    {
        printf("Failed to Open Listener. Port: %d. Error: %s\n", port, strerror(errno));
        return listenerSocket;
    }

    bzero((char *)&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    result = bind(listenerSocket, (struct sockaddr *)&address, sizeof(address));

    if (result != 0)
    {
        printf("Failed to Bind Listener. Port: %d. Error: %s\n", port, strerror(errno));
        return result;
    }

    result = listen(listenerSocket, LISTENER_BACKLOG);

    if (result != 0)
    {
        printf("Failed to Mark Listener. Port: %d. Error: %s\n", port, strerror(errno));
        return result;
    }

    flags = fcntl(listenerSocket, F_GETFL, 0);

    if (flags < 0)
    {
        printf("Failed to get Listener flags. Port: %d. Error: %s\n", port, strerror(errno));
        return result;
    }

    result = fcntl(listenerSocket, F_SETFL, flags | O_NONBLOCK);

    if (result < 0)
    {
        printf("Failed to set Listener as non blocking. Port: %d. Error: %s\n", port, strerror(errno));
        return result;
    }

    return listenerSocket;
}

int PortForwarder::openOutput(int port)
{
    int ouputSocket, result;
    struct hostent *forward;
    struct sockaddr_in address;

    forward = gethostbyname("localhost");

    if (forward == NULL)
    {
        printf("Failed to get localhost for output.\n");
        return -1;
    }

    bzero((char *)&address, sizeof(address));
    address.sin_family = AF_INET;
    bcopy((char *)forward->h_addr, (char *)&address.sin_addr.s_addr, forward->h_length);
    address.sin_port = htons(port);

    ouputSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (ouputSocket < 0)
    {
        printf("Failed to create Output socket. Port: %d. Error: %s\n", port, strerror(errno));
        return ouputSocket;
    }

    result = connect(ouputSocket, (struct sockaddr *)&address, sizeof(address));

    if (result != 0)
    {
        printf("Failed to open Output socket. Port: %d. Error: %s\n", port, strerror(errno));
        return result;
    }

    return ouputSocket;
}

bool PortForwarder::portCheck(int port, int maxRetries)
{
    int socketCheck;
    struct sockaddr_in serverAddress;
    struct hostent *server;

    server = gethostbyname("localhost");

    if (server == NULL)
    {
        printf("Failed to get localhost for output.\n");
        return false;
    }

    socketCheck = socket(AF_INET, SOCK_STREAM, 0);

    if (socketCheck < 0)
    {
        printf("Failed to open socket for checking port: %d. Error: %s\n", port, strerror(errno));
        return false;
    }

    bzero((char *)&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serverAddress.sin_addr.s_addr,
          server->h_length);

    serverAddress.sin_port = htons(port);

    int retries = 0;

    // Loop until port opens
    while (
        connect(socketCheck, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0 &&
        retries++ < maxRetries)
    {
        sleep_for(milliseconds(RETRY_TIMEOUT));
    };

    close(socketCheck);
    return retries <= maxRetries;
}

void PortForwarder::closeSocket(int socket)
{
    char buffer[FORWARDER_BUFFER_SIZE];

    // Assuring socket is empty before closing
    while (read(socket, buffer, FORWARDER_BUFFER_SIZE) > 0)
    {
    }

    close(socket);
}

PortForwarder::~PortForwarder()
{
    if (!isStopped())
    {
        stop();
    }

    delete portLock;
}

bool PortForwarder::isBlocked()
{
    lock_guard<mutex> guard(*portLock);
    return blocked;
}

bool PortForwarder::isStopped()
{
    lock_guard<mutex> guard(*portLock);
    return stopped;
}

void PortForwarder::listener(int source, int destination)
{
    struct pollfd pfd[1];

    pfd[0].fd = source;
    pfd[0].events = POLLIN;

    char buffer[FORWARDER_BUFFER_SIZE];
    int readLength, writeLength, written;

    while (!isStopped())
    {
        poll(pfd, 1, RETRY_TIMEOUT);
        if ((pfd[0].revents & (POLLIN)))
        {
            readLength = read(source, buffer, FORWARDER_BUFFER_SIZE);
            written = 0;
            if (readLength < 0)
            {
                break;
            }
            while (!isBlocked() && written < readLength)
            {
                writeLength = write(destination, buffer + written, readLength - written);

                if (writeLength == -1)
                {
                    break;
                }

                written += writeLength;
            }
        }
    }

    // We shutdown our ports here, we will close them once all threads have closed.
    shutdown(source, SHUT_RD);
    shutdown(destination, SHUT_WR);
}

void PortForwarder::main()
{
    sockets.push_back(outSocket);

    while (!isStopped())
    {
        int clientSocket;

        clientSocket = accept(inSocket, NULL, NULL);

        if (clientSocket >= 0)
        {
            sockets.push_back(clientSocket);
        }

        if (isStopped())
        {
            break;
        }

        if (clientSocket >= 0)
        {
            threads.push_back(new thread(&PortForwarder::listener, this, clientSocket, outSocket));
            threads.push_back(new thread(&PortForwarder::listener, this, outSocket, clientSocket));
        }

        sleep_for(milliseconds(RETRY_TIMEOUT));
    }
}

int PortForwarder::start()
{
    portLock->lock();
    stopped = false;
    portLock->unlock();

    if ((inSocket = PortForwarder::openListener(inPort)) < 0)
    {
        printf("Failed to open listener, stopping\n");
        stop();
        return -1;
    }

    if ((outSocket = PortForwarder::openOutput(outPort)) < 0)
    {
        printf("Failed to open output, stopping\n");
        stop();
        return -1;
    }
    threads.push_back(new thread(&PortForwarder::main, this));
    return 0;
}

void PortForwarder::stop()
{
    portLock->lock();
    stopped = true;
    portLock->unlock();

    for (auto running : threads)
    {
        running->join();
        delete running;
    }

    threads.clear();

    for (auto socket : sockets)
    {
        PortForwarder::closeSocket(socket);
    }

    sockets.clear();

    if (inSocket >= 0)
    {
        close(inSocket);
        inSocket = -1;
    }
}

void PortForwarder::block()
{
    lock_guard<mutex> guard(*portLock);
    blocked = true;
}

void PortForwarder::unblock()
{
    lock_guard<mutex> guard(*portLock);
    blocked = false;
}