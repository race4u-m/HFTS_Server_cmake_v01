#include "MqttClient.h"

#include <mqtt/async_client.h>
#include <memory>
#include <atomic>
#include <iostream>
#include <chrono>

// 전방 선언
struct MqttImpl;

// Paho MQTT 콜백 클래스
class MqttCallback : public virtual mqtt::callback
{
public:
    MqttCallback(MqttClient& parent, MqttImpl& impl)
        : parent_(parent), impl_(impl) {
    }

    void connected(const std::string&) override;
    void connection_lost(const std::string&) override {}
    void message_arrived(mqtt::const_message_ptr msg) override;

private:
    MqttClient& parent_;
    MqttImpl& impl_;
};

// PIMPL 실제 구현체
struct MqttImpl
{
    mqtt::async_client cli;
    mqtt::connect_options connOpts;
    std::atomic<bool> running{ false };

    // 콜백 수명 보장 (cli 내부가 참조할 수 있으므로 멤버로 잡아둠)
    std::shared_ptr<MqttCallback> cb;

    explicit MqttImpl(const MqttClient::Config& cfg)
        : cli(cfg.serverUri, cfg.clientId)
    {
        connOpts.set_clean_session(cfg.cleanSession);
    }
};

void MqttCallback::connected(const std::string&)
{
    //  구조 정리:
    // connect 콜백에서 subscribe 하지 않음.
    // subscribe는 start()에서 "토큰 wait"까지 확실히 완료시키는 쪽이 디버깅/제어가 쉬움.
}

void MqttCallback::message_arrived(mqtt::const_message_ptr msg)
{
    if (!msg) return;

    // onMessage()는 std::function이라 비어있으면 false로 평가됨
    if (parent_.onMessage())
        parent_.onMessage()(msg->get_topic(), msg->to_string());
}

MqttClient::MqttClient(Config cfg)
    : cfg_(std::move(cfg))
{
    auto impl = std::make_unique<MqttImpl>(cfg_);
    impl_ = impl.release();
}

MqttClient::~MqttClient()
{
    stop();
    delete static_cast<MqttImpl*>(impl_);
    impl_ = nullptr;
}

void MqttClient::setOnMessage(OnMessage cb)
{
    onMsg_ = std::move(cb);
}

// MqttClient 시작: 브로커 연결 + 토픽 구독까지 완료
bool MqttClient::start(std::string& err)
{
    err.clear();

    auto* impl = static_cast<MqttImpl*>(impl_);
    if (!impl) {
        err = "impl is null";
        return false;
    }

    if (impl->running.load()) {
        std::cout << "[MQTT] already running" << std::endl;
        return true;
    }

    try
    {
        // 0) 콜백 먼저 준비/등록 (메시지 도착 즉시 받기 위해)
        impl->cb = std::make_shared<MqttCallback>(*this, *impl);
        impl->cli.set_callback(*impl->cb);

        // 1) CONNECT
        std::cout << "[MQTT] connecting to " << cfg_.serverUri << " ..." << std::endl;
        auto connTok = impl->cli.connect(impl->connOpts);
        connTok->wait();
        std::cout << "[MQTT] connected" << std::endl;

        // 2) SUBSCRIBE
        std::cout << "[MQTT] subscribing topic=" << cfg_.topic << " qos=" << cfg_.qos << std::endl;

        auto subTok = impl->cli.subscribe(cfg_.topic, cfg_.qos);

        // wait_for가 먹히려면 토큰을 "응답 토큰"으로 받아야 하는데,
        // Paho C++에서는 subscribe()가 delivery_token을 돌려주기도 해서
        // 일부 환경에서 wait_for가 동작이 애매할 수 있음.
        // 그래서 여기선 가장 확실한 wait()로 완료 보장.
        subTok->wait();

        std::cout << "[MQTT] subscribed OK" << std::endl;

        impl->running.store(true);
        return true;
    }
    catch (const mqtt::exception& e)
    {
        err = std::string("MQTT start failed: ") + e.what();
        impl->running.store(false);
        std::cout << "[MQTT] EXCEPTION: " << err << std::endl;
        return false;
    }
}

void MqttClient::stop()
{
    auto* impl = static_cast<MqttImpl*>(impl_);
    if (!impl) return;
    if (!impl->running.load()) return;

    try {
        impl->cli.disconnect()->wait();
    }
    catch (...) {
        // 종료 중 예외는 무시 (프로세스 종료 안정성 우선)
    }

    impl->running.store(false);
}
