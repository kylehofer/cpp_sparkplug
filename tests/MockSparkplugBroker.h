#ifndef TESTS_MOCKSPARKPLUGBROKER
#define TESTS_MOCKSPARKPLUGBROKER

#include "gmock/gmock.h"
#include "SparkplugBroker.h"

class MockSparkplugBroker : public SparkplugBroker {
private:
protected:
public:
    MockSparkplugBroker() : MockSparkplugBroker(NULL, NULL) {};
    MockSparkplugBroker(BrokerEventHandler *handler, BrokerOptions* options) : SparkplugBroker(handler, options) { };

    MOCK_METHOD(int, requestPublish, (PublishRequest* publishRequest), (override));
    MOCK_METHOD(int, clientConnect, (), (override));
    MOCK_METHOD(int, clientDisconnect, (), (override));
    MOCK_METHOD(int, subscribeToPrimaryHost, (), (override));
    MOCK_METHOD(int, subscribeToCommands, (), (override));
    MOCK_METHOD(int, unsubscribeToCommands, (), (override));
    MOCK_METHOD(int, publishMessage, (const char* topic, uint8_t* buffer, size_t length, DeliveryToken* token), (override));
    MOCK_METHOD(int, configureClient, (BrokerOptions* options), (override));


    void connect()
    {
        SparkplugBroker::connected();
    }

    void active()
    {
        SparkplugBroker::activated();
    }

    BrokerTopicOptions *getTopics()
    {
        return topics;
    }

    int publish(PublishRequest* publishRequest)
    {
        return SparkplugBroker::publish(publishRequest);
    }
};

#endif /* TESTS_MOCKSPARKPLUGBROKER */
