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
