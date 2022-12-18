#ifndef TESTS_MOCKSPARKPLUGCLIENT
#define TESTS_MOCKSPARKPLUGCLIENT

#include "gmock/gmock.h"
#include "clients/SparkplugClient.h"

class MockSparkplugClient : public SparkplugClient
{
private:
    bool connected = false;

protected:
public:
    MockSparkplugClient() : MockSparkplugClient(NULL, NULL){};
    MockSparkplugClient(ClientEventHandler *handler, ClientOptions *options) : SparkplugClient(handler, options){};

    MOCK_METHOD(int, request, (PublishRequest * publishRequest), (override));
    MOCK_METHOD(int, clientConnect, (), (override));
    MOCK_METHOD(int, clientDisconnect, (), (override));
    MOCK_METHOD(int, subscribeToPrimaryHost, (), (override));
    MOCK_METHOD(int, subscribeToCommands, (), (override));
    MOCK_METHOD(int, unsubscribeToCommands, (), (override));
    MOCK_METHOD(int, publishMessage, (const char *topic, uint8_t *buffer, size_t length, DeliveryToken *token), (override));
    MOCK_METHOD(int, configureClient, (ClientOptions * options), (override));

    void connect()
    {
        connected = true;
        SparkplugClient::connected();
    }

    void active()
    {
        SparkplugClient::activated();
    }

    ClientTopicOptions *getTopics()
    {
        return topics;
    }

    int processRequest(PublishRequest *publishRequest)
    {
        return SparkplugClient::processRequest(publishRequest);
    }

    void sync()
    {
    }

    bool isConnected()
    {
        return connected;
    }
};

#endif /* TESTS_MOCKSPARKPLUGCLIENT */
