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
# 공통: JSON (Windows(vcpkg) / Linux(apt) 모두 가능)
# ===============================
# - Windows(vcpkg): nlohmann_json CONFIG
# - Linux(apt):     보통 CMake config 제공
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(HFTS_Server_cmake_v01 PRIVATE nlohmann_json::nlohmann_json)

# ===============================
# OS별: MQTT(Paho) / SQLite 링크 방식 분기
# ===============================
if (WIN32)
    # --------------------------------
    # Windows: vcpkg 패키지 사용
    # --------------------------------
    find_package(PahoMqttCpp CONFIG REQUIRED)
    find_package(unofficial-sqlite3 CONFIG REQUIRED)

    target_link_libraries(HFTS_Server_cmake_v01
        PRIVATE
            PahoMqttCpp::paho-mqttpp3
            unofficial::sqlite3::sqlite3
    )

    # Windows에서 DLL import 매크로 필요(네 코드와 동일)
    target_compile_definitions(HFTS_Server_cmake_v01 PRIVATE PAHO_MQTTPP_IMPORTS)

else()
    # --------------------------------
    # Linux: apt 패키지 사용 (가장 안정)
    # --------------------------------
    # SQLite3
    find_package(SQLite3 REQUIRED)
    target_link_libraries(HFTS_Server_cmake_v01 PRIVATE SQLite::SQLite3)

    # Paho MQTT: pkg-config로 찾기
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PAHO_MQTTPP REQUIRED IMPORTED_TARGET paho-mqttpp3)

    # paho-mqttpp3 (C++ wrapper)
    target_link_libraries(HFTS_Server_cmake_v01 PRIVATE PkgConfig::PAHO_MQTTPP)

    # async_client가 필요로 하는 C 라이브러리(ssl 버전)
    # 대부분 자동으로 잡히지만, 환경에 따라 명시하면 더 안전
    target_link_libraries(HFTS_Server_cmake_v01 PRIVATE paho-mqtt3as)

endif()
