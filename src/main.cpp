#include <iostream>
#include <chrono>

#include "MqttClient.h"
#include "tpMeasParser.h"
#include "DbWriter.h"
#include "models.h"


static double unixTimeNow()    //  ts 만들기(서버 수신 시각)
{
    using namespace std::chrono;
    return duration<double>(system_clock::now().time_since_epoch()).count();
}

int main()
{
    std::string err;

    // 1) DB 초기화 (윈도우 테스트 경로)
    DbWriter db;
    if (!db.init("C:/hfts/db", err))
    {
        std::cerr << "[DB] init failed: " << err << "\n";
        return 1;
    }

    // 2) MQTT 설정
    MqttClient::Config cfg;
    cfg.serverUri = "tcp://127.0.0.1:1883";
    cfg.clientId = "HFTS_Server_cmake_v01";
    cfg.topic = "HFTS/tpmeas";
    cfg.qos = 1;

    MqttClient cli(cfg);

    // 3) 메시지 수신 처리
    cli.setOnMessage([&](const std::string& topic, const std::string& payload)
        {
            (void)topic;

            TpMeas meas;
            std::string perr;
            if (!tpMeasParser::parse(payload, meas, perr))
            {
                std::cerr << "[PARSE] " << perr << " | payload=" << payload << "\n";
                return;
            }

            TpMeasDbRow row;
            row.ts = unixTimeNow();   // 여기서 ts 찍음(수신 시각)
            row.DTNO = meas.DTNO;
            row.v = meas.v;

            std::string derr;
            if (!db.insert(row, derr))
            {
                std::cerr << "[DB] insert failed: " << derr << "\n";
                return;
            }

            std::cout << "[OK] saved DTNO=" << row.DTNO << " ts=" << row.ts << "\n";
        });

    // 4) 시작
    if (!cli.start(err))
    {
        std::cerr << err << "\n";
        return 1;
    }

    std::cout << "Running... topic=" << cfg.topic << "\n";
    std::cout << "Press Enter to quit.\n";
    std::cin.get();

    cli.stop();
    return 0;
}
