#pragma once
#include <functional>
#include <string>
#include <mqtt/async_client.h>

class MqttClient
{
public:
    using MessageHandler =
        std::function<void(const std::string&, const std::string&)>;

    MqttClient(std::string serverUri, std::string clientId);
    void SetMessageHandler(MessageHandler cb);

    void ConnectAndSubscribe(const std::string& topicFilter, int qos = 1);
    void Disconnect();

private:
    mqtt::async_client cli_;
    MessageHandler onMsg_;

    class Callback : public virtual mqtt::callback
    {
    public:
        explicit Callback(MqttClient& parent) : parent_(parent) {}
        void message_arrived(mqtt::const_message_ptr msg) override;
    private:
        MqttClient& parent_;
    };

    Callback cb_;
};
