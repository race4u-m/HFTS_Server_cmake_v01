#pragma once

#include <functional>
#include <string>

class MqttClient
{
public:
    struct Config
    {
        std::string serverUri;   // 예: tcp://127.0.0.1:1883
        std::string clientId;    // 예: HFTS_Server
        std::string topic;       // 예: HFTS/tpmeas
        int qos = 1;
        bool cleanSession = true;
    };

    using OnMessage = std::function<void(const std::string& topic,
        const std::string& payload)>;

    explicit MqttClient(Config cfg);
    ~MqttClient();

    void setOnMessage(OnMessage cb);

    // cpp에서 사용 중이라 반드시 있어야 함
    const Config& config() const { return cfg_; }
    const OnMessage& onMessage() const { return onMsg_; }

    bool start(std::string& err);

    // cpp 최신 버전과 동일하게 인자 없는 stop()
    void stop();

private:
    Config cfg_;
    OnMessage onMsg_;
    void* impl_ = nullptr;  // PIMPL (MqttImpl*)
};
