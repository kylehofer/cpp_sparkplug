#ifndef TESTS_MOCKSPARKPLUGCLIENT
#define TESTS_MOCKSPARKPLUGCLIENT

#include "gmock/gmock.h"
#include "SparkplugClient.h"

class MockSparkplugClient : public SparkplugClient {
private:
protected:
public:
    MockSparkplugClient() : MockSparkplugClient(NULL, NULL) {};
    MockSparkplugClient(ClientEventHandler *handler, ClientOptions* options) : SparkplugClient(handler, options) { };

    MOCK_METHOD(int, requestPublish, (PublishRequest* publishRequest), (override));
    MOCK_METHOD(int, clientConnect, (), (override));
    MOCK_METHOD(int, clientDisconnect, (), (override));
    MOCK_METHOD(int, subscribeToPrimaryHost, (), (override));
    MOCK_METHOD(int, subscribeToCommands, (), (override));
    MOCK_METHOD(int, unsubscribeToCommands, (), (override));
    MOCK_METHOD(int, publishMessage, (const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token), (override));
    MOCK_METHOD(int, configureClient, (ClientOptions* options), (override));


    void connect()
    {
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

    int publish(PublishRequest* publishRequest)
    {
        return SparkplugClient::publish(publishRequest);
    }
};

#endif /* TESTS_MOCKSPARKPLUGCLIENT */
