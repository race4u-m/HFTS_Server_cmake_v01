#pragma once
#include <string>
#include "models.h"

class tpMeasParser
{
public:
    // payload(JSON ¹®ÀÚ¿­) -> TpMeas ÆÄ½Ì
    static bool parse(const std::string& payload, TpMeas& out, std::string& err);
};
