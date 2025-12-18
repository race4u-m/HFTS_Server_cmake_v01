#pragma once
#include <cstdint>

struct tpMeas
{
    std::int64_t DataNo = 0;
    double SecFrom1900 = 0.0;

    double P_N2 = 0.0;
    double T_Line1 = 0.0;
    double T_Line2 = 0.0;
    double P_Line1 = 0.0;
    double P_Line2 = 0.0;
    double H_Atm = 0.0;
    double T_Atm = 0.0;

    double T_Tank1 = 0.0;
    double T_Tank2 = 0.0;
    double T_Tank3 = 0.0;

    double H2D_1 = 0.0;
    double H2D_2 = 0.0;
    double IRF_1 = 0.0;
    double IRF_2 = 0.0;

    double Weight = 0.0;
    double Q_Ins = 0.0;
    double Q_Tot = 0.0;

    std::int64_t DIO = 0;
    double Tare_Q_Tot = 0.0;
    double Tare_Weight = 0.0;
    double DP = 0.0;
};
