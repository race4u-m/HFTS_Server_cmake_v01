#include "MqttClient.h"
#include <iostream>

MqttClient::MqttClient(std::string serverUri, std::string clientId)
    : cli_(serverUri, clientId), cb_(*this)
{
    cli_.set_callback(cb_);
}

void MqttClient::SetMessageHandler(MessageHandler cb)
{
    onMsg_ = std::move(cb);
}

void MqttClient::ConnectAndSubscribe(const std::string& topicFilter, int qos)
{
    mqtt::connect_options opts;
    opts.set_clean_session(true);

    std::cout << "[MQTT] Connecting...\n";
    cli_.connect(opts)->wait();

    std::cout << "[MQTT] Subscribing: " << topicFilter << "\n";
    cli_.subscribe(topicFilter, qos)->wait();
}

void MqttClient::Disconnect()
{
    try { cli_.disconnect()->wait(); }
    catch (...) {}
}

void MqttClient::Callback::message_arrived(mqtt::const_message_ptr msg)
{
    if (msg && parent_.onMsg_)
        parent_.onMsg_(msg->get_topic(), msg->to_string());
}
