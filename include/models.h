#pragma once
#include <array>
#include <cstdint>

constexpr int TP_V_COUNT = 22;

// MQTT에서 받은 측정 데이터(파싱 결과)
struct TpMeas
{
    int64_t DTNO = 0;                          // 데이터 번호(추천 키)
    std::array<double, TP_V_COUNT> v{};        // 22개 값 고정
};

// DB에 넣을 레코드(저장용)
struct TpMeasDbRow
{
    double ts = 0.0;                           // 서버 수신 시각 (Unix time, seconds)
    int64_t DTNO = 0;
    std::array<double, TP_V_COUNT> v{};
};
