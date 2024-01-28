#ifndef TESTS_UTILS_TESTMQTTCLIENT
#define TESTS_UTILS_TESTMQTTCLIENT

#include "clients/CppMqttClient.h"
#include "TestTcpClient.h"

class TestMqttClient : public CppMqttClient
{
private:
    TestTcpClient tcpClient;

protected:
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
    TestMqttClient(ClientEventHandler *handler, ClientOptions *options) : CppMqttClient(handler, options)
    {
    }
};

#endif /* TESTS_UTILS_TESTMQTTCLIENT */
