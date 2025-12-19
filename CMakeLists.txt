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
# vcpkg 패키지
# ===============================
find_package(nlohmann_json CONFIG REQUIRED)
find_package(PahoMqttCpp CONFIG REQUIRED)
#find_package(PahoMqttC   CONFIG REQUIRED)
#find_package(sqlite3 CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# ===============================
# 링크
# ===============================
target_link_libraries(HFTS_Server_cmake_v01
    PRIVATE
        PahoMqttCpp::paho-mqttpp3 
        nlohmann_json::nlohmann_json
        unofficial::sqlite3::sqlite3
)

#target_compile_definitions(HFTS_Server_cmake_v01 PRIVATE PAHO_MQTTPP_IMPORTS)

if (WIN32)
  target_compile_definitions(HFTS_Server_cmake_v01 PRIVATE PAHO_MQTTPP_IMPORTS)
endif()