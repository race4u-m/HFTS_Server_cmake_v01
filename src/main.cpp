

#include <iostream>   // std::cout, std::cin 
#include <string>     // std::string

#include "MqttClient.h"
#include "tpMeasParser.h"

int main()
{    
    const std::string serverUri = "tcp://127.0.0.1:1883";    
    const std::string deviceId = "HF";

    
    const std::string topic = "HFTS_" + deviceId + "/tpMeas";

    
    // - 생성자에 serverUri와 clientId를 전달    
    // clientId는 MQTT에서 클라이언트를 구분하는 ID.
    // (같은 브로커에 동일 clientId가 동시에 접속하면 충돌할 수 있음)
    MqttClient mqtt(serverUri, "mqttCli_Server");

    // -----------------------------
    // 4) 메시지 수신 콜백(Callback) 등록
    // -----------------------------

    // mqtt.SetMessageHandler( ... ) 는
    // "메시지가 도착했을 때 실행할 함수"를 등록하는 역할.
    //
    // 여기서 [&](...) { ... } 형태는 "람다(lambda) 함수" 문법이다.
    // - 람다는 이름 없는(익명) 함수를 즉석에서 만드는 문법.
    //
    // [&] 의미:
    // - 바깥쪽 변수(topic, mqtt 등)를 "참조(reference)"로 캡처(capture)하겠다.
    // - 참조 캡처는 바깥 변수를 복사하지 않고 그대로 사용한다.
    //
    // (const std::string& t, const std::string& payload)
    // - 콜백 함수가 받을 인자 2개
    // - &는 참조(reference): 큰 문자열을 복사하지 않고 "원본을 가리키는 형태"로 받음(성능↑)
    // - const는 "이 함수 안에서 t/payload를 수정하지 않겠다"는 뜻
    mqtt.SetMessageHandler(
        [&](const std::string& t, const std::string& payload)
        {
            // -----------------------------
            // JSON payload를 tpMeas 구조체로 파싱
            // -----------------------------
            
            // - 측정값이 여기로 채워질 예정
            tpMeas meas;

            
            // - 파싱 실패 시 이유(에러 메시지)를 담을 문자열
            std::string err;

            // tpMeasParser::TryParseJson(...)
            // - 클래스 이름::함수이름  형태는 "정적(static) 함수 호출"
            // - 객체를 만들지 않고도 호출 가능
            // - 성공하면 true, 실패하면 false를 리턴
            if (!tpMeasParser::TryParseJson(payload, meas, err))
            {
                std::cout << "[tpMeas] Parse FAIL: " << err << "\n";
                return;
            }

            // -----------------------------
            // 4-2) 파싱된 값 일부를 콘솔에 출력
            // -----------------------------
            std::cout
                << "[tpMeas] topic=" << t
                << " DataNo=" << meas.DataNo
                << " SecFrom1900=" << meas.SecFrom1900
                << " P_N2=" << meas.P_N2
                << " T_Line1=" << meas.T_Line1
                << " DIO=" << meas.DIO
                << "\n";
        }
    );
        
    try
    {
        // - qos=1 : "최소 1번 전달" 보장(중복 수신 가능)
        mqtt.ConnectAndSubscribe(topic, 1);

        // 여기까지 왔다면 연결 및 구독이 성공한 것.
        std::cout << "Subscribed. Press ENTER to exit...\n";

        // std::cin.get();
        // - 사용자 입력(엔터)을 1글자 읽는다.
        // - 즉, 엔터를 누를 때까지 프로그램이 종료되지 않고 대기한다.
        // - 그동안 MQTT 메시지가 오면 콜백이 계속 실행됨
        std::cin.get();

        // 종료 전에 연결을 끊는다.
        mqtt.Disconnect();
    }
    catch (const std::exception& e)
    {
        // 표준 예외(std::exception)는 e.what()으로 에러 메시지를 얻을 수 있다.
        std::cout << "Exception: " << e.what() << "\n";

        // main()에서 1을 리턴하면 "비정상 종료" 의미로 많이 사용한다.
        return 1;
    }

    // 정상 종료: 0 리턴
    return 0;
}
