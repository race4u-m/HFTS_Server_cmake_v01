#pragma once
#include <string>
#include "models.h"

class tpMeasParser
{
public:
    // JSON payload ¡æ tpMeas
    static bool TryParseJson(
        const std::string& payload,
        tpMeas& out,
        std::string& err
    );
};
