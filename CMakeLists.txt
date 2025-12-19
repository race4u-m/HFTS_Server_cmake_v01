cmake_minimum_required(VERSION 3.20)

project(HFTS_Server_cmake_v01 LANGUAGES CXX)

# ===============================
# C++ 표준
# ===============================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===============================
# 실행 파일
# ===============================
add_executable(HFTS_Server_cmake_v01
    src/main.cpp
    src/MqttClient.cpp
    src/tpMeasParser.cpp
    src/DbWriter.cpp
)

# ===============================
# include 경로
# ===============================
target_include_directories(HFTS_Server_cmake_v01
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ===============================
# JSON (공통)
# - Windows(vcpkg)에서도, Linux(apt)에서도 nlohmann_json config가 보통 잡힘
# ===============================
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(HFTS_Server_cmake_v01 PRIVATE nlohmann_json::nlohmann_json)

# ===============================
# OS별: MQTT(Paho) / SQLite 링크
# ===============================
if (WIN32)
    # --------------------------------------------------
    # Windows: vcpkg 패키지 사용 (네가 윈도우에서 성공한 방식)
    # --------------------------------------------------
    find_package(PahoMqttCpp CONFIG REQUIRED)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)

    target_link_libraries(HFTS_Server_cmake_v01
        PRIVATE
            PahoMqttCpp::paho-mqttpp3
            unofficial::sqlite3::sqlite3
    )

    # Windows DLL import 매크로 (네 기존 코드 유지)
    target_compile_definitions(HFTS_Server_cmake_v01 PRIVATE PAHO_MQTTPP_IMPORTS)

else()
    # --------------------------------------------------
    # Linux: apt 설치 기준 (어제 리눅스에서 되던 스타일로 "가장 단순/안정")
    # --------------------------------------------------

    # SQLite3 (apt)
    find_package(SQLite3 REQUIRED)
    target_link_libraries(HFTS_Server_cmake_v01 PRIVATE SQLite::SQLite3)

    # Paho MQTT (apt)
    # Ubuntu에선 PahoMqttCppConfig.cmake가 없을 수 있어서 find_package(PahoMqttCpp) 쓰지 않음
    # 라이브러리를 직접 링크하는 방식이 가장 안정적
    target_link_libraries(HFTS_Server_cmake_v01
        PRIVATE
            paho-mqttpp3
            paho-mqtt3as
    )

endif()
