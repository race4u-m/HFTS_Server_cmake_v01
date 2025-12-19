#pragma once
#include <mutex>
#include <string>
#include "models.h"

class DbWriter
{
public:
    DbWriter();
    ~DbWriter();

    // dbDir: DB 파일 저장 폴더 (예: C:/hfts/db 또는 ./db)
    bool init(const std::string& dbDir, std::string& err);

    // 월별 파일(tpmeas_YYYY-MM.sqlite)에 저장
    bool insert(const TpMeasDbRow& row, std::string& err);

private:
    std::string dbDir_;
    std::mutex mtx_;

    std::string monthKeyFromTs_(double ts);           // "YYYY-MM"
    std::string dbPathForMonth_(const std::string& monthKey);
};
