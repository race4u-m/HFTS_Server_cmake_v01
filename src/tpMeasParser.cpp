#include "tpMeasParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static bool toInt64Safe(const json& j, int64_t& out)
{
    try
    {
        if (j.is_number_integer()) { out = j.get<int64_t>(); return true; }
        if (j.is_number_unsigned()) { out = static_cast<int64_t>(j.get<uint64_t>()); return true; }
        if (j.is_number_float()) { out = static_cast<int64_t>(j.get<double>()); return true; }
        if (j.is_string()) { out = std::stoll(j.get<std::string>()); return true; }
    }
    catch (...) {}
    return false;
}

static bool toDoubleSafe(const json& j, double& out)
{
    try
    {
        if (j.is_number()) { out = j.get<double>(); return true; }
        if (j.is_string()) { out = std::stod(j.get<std::string>()); return true; }
    }
    catch (...) {}
    return false;
}

bool tpMeasParser::parse(const std::string& payload, TpMeas& out, std::string& err)
{
    out = TpMeas{};
    err.clear();

    json j;
    try
    {
        j = json::parse(payload);
    }
    catch (const std::exception& e)
    {
        err = std::string("JSON parse failed: ") + e.what();
        return false;
    }

    // ver는 이번 설계에서 제외(있어도 무시)
    // DTNO 사용
    if (!j.contains("DTNO"))
    {
        err = "Missing field: DTNO";
        return false;
    }
    if (!toInt64Safe(j["DTNO"], out.DTNO))
    {
        err = "DTNO type invalid";
        return false;
    }

    // v : 22개 배열
    if (!j.contains("v") || !j["v"].is_array())
    {
        err = "Missing or invalid field: v (must be array)";
        return false;
    }

    const auto& a = j["v"];
    if (a.size() != TP_V_COUNT)
    {
        err = "v array size must be 22";
        return false;
    }

    for (int i = 0; i < TP_V_COUNT; ++i)
    {
        double val = 0.0;
        if (!toDoubleSafe(a[i], val))
        {
            err = "v[" + std::to_string(i) + "] invalid";
            return false;
        }
        out.v[i] = val;
    }

    return true;
}
