#include "DbWriter.h"
#include <sqlite3.h>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>

DbWriter::DbWriter() = default;
DbWriter::~DbWriter() = default;

static bool ensureDir(const std::string& dir, std::string& err)
{
    try
    {
        std::filesystem::create_directories(dir);
        return true;
    }
    catch (const std::exception& e)
    {
        err = std::string("create_directories failed: ") + e.what();
        return false;
    }
}

static bool execSql(sqlite3* db, const char* sql, std::string& err)
{
    char* msg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &msg);
    if (rc != SQLITE_OK)
    {
        err = msg ? msg : "sqlite3_exec failed";
        sqlite3_free(msg);
        return false;
    }
    return true;
}

static bool ensureSchema(sqlite3* db, std::string& err)
{
    // ts REAL 은 sqlite에서 "실수" 저장 타입
    // dtno INTEGER는 64-bit 정수 저장 가능
    const char* sql =
        "PRAGMA journal_mode=WAL;"
        "PRAGMA synchronous=NORMAL;"
        "CREATE TABLE IF NOT EXISTS tpmeas ("
        "  ts   REAL NOT NULL,"
        "  dtno INTEGER NOT NULL,"
        "  v0 REAL, v1 REAL, v2 REAL, v3 REAL, v4 REAL, v5 REAL, v6 REAL, v7 REAL, v8 REAL, v9 REAL,"
        "  v10 REAL, v11 REAL, v12 REAL, v13 REAL, v14 REAL, v15 REAL, v16 REAL, v17 REAL, v18 REAL, v19 REAL,"
        "  v20 REAL, v21 REAL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_tpmeas_dtno ON tpmeas(dtno);";

    return execSql(db, sql, err);
}

std::string DbWriter::monthKeyFromTs_(double ts)
{
    std::time_t t = static_cast<std::time_t>(ts);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m");
    return oss.str();
}

std::string DbWriter::dbPathForMonth_(const std::string& monthKey)
{
    std::filesystem::path p(dbDir_);
    p /= ("tpmeas_" + monthKey + ".sqlite");
    return p.string();
}

bool DbWriter::init(const std::string& dbDir, std::string& err)
{
    std::lock_guard<std::mutex> lock(mtx_);
    dbDir_ = dbDir;
    return ensureDir(dbDir_, err);
}

bool DbWriter::insert(const TpMeasDbRow& row, std::string& err)
{
    err.clear();

    std::lock_guard<std::mutex> lock(mtx_);

    const std::string monthKey = monthKeyFromTs_(row.ts);
    const std::string dbPath = dbPathForMonth_(monthKey);

    sqlite3* db = nullptr;
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        err = db ? sqlite3_errmsg(db) : "sqlite3_open failed";
        if (db) sqlite3_close(db);
        return false;
    }

    if (!ensureSchema(db, err))
    {
        sqlite3_close(db);
        return false;
    }

    const char* sql =
        "INSERT INTO tpmeas (ts, dtno,"
        " v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,"
        " v10,v11,v12,v13,v14,v15,v16,v17,v18,v19,"
        " v20,v21"
        ") VALUES (?,?,"
        " ?,?,?,?,?,?,?,?,?,?,"
        " ?,?,?,?,?,?,?,?,?,?,"
        " ?,?"
        ");";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        err = sqlite3_errmsg(db);
        sqlite3_close(db);
        return false;
    }

    int idx = 1;
    sqlite3_bind_double(stmt, idx++, row.ts);
    sqlite3_bind_int64(stmt, idx++, row.DTNO);
    for (int i = 0; i < TP_V_COUNT; ++i)
        sqlite3_bind_double(stmt, idx++, row.v[i]);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        err = sqlite3_errmsg(db);
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    return true;
}
