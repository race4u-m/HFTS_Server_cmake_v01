#include "tpMeasParser.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static bool RequireField(const json& j, const char* key)
{
    return j.contains(key) && !j.at(key).is_null();
}

bool tpMeasParser::TryParseJson(
    const std::string& payload,
    tpMeas& out,
    std::string& err
)
{
    try
    {
        auto j = json::parse(payload);

        // 필수 필드
        if (!RequireField(j, "DataNo") || !RequireField(j, "SecFrom1900"))
        {
            err = "Missing required field: DataNo or SecFrom1900";
            return false;
        }

        out.DataNo = j.value("DataNo", 0LL);
        out.SecFrom1900 = j.value("SecFrom1900", 0.0);

        out.P_N2 = j.value("P_N2", 0.0);
        out.T_Line1 = j.value("T_Line1", 0.0);
        out.T_Line2 = j.value("T_Line2", 0.0);
        out.P_Line1 = j.value("P_Line1", 0.0);
        out.P_Line2 = j.value("P_Line2", 0.0);
        out.H_Atm = j.value("H_Atm", 0.0);
        out.T_Atm = j.value("T_Atm", 0.0);

        out.T_Tank1 = j.value("T_Tank1", 0.0);
        out.T_Tank2 = j.value("T_Tank2", 0.0);
        out.T_Tank3 = j.value("T_Tank3", 0.0);

        out.H2D_1 = j.value("H2D_1", 0.0);
        out.H2D_2 = j.value("H2D_2", 0.0);
        out.IRF_1 = j.value("IRF_1", 0.0);
        out.IRF_2 = j.value("IRF_2", 0.0);

        out.Weight = j.value("Weight", 0.0);
        out.Q_Ins = j.value("Q_Ins", 0.0);
        out.Q_Tot = j.value("Q_Tot", 0.0);

        out.DIO = j.value("DIO", 0LL);
        out.Tare_Q_Tot = j.value("Tare_Q_Tot", 0.0);
        out.Tare_Weight = j.value("Tare_Weight", 0.0);
        out.DP = j.value("DP", 0.0);

        err.clear();
        return true;
    }
    catch (const std::exception& e)
    {
        err = e.what();
        return false;
    }
}
