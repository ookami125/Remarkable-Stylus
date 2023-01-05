#pragma once
#include <Windows.h>
#include <string>

template<typename T>
inline T min(T op1, T op2)
{
    return (op1 < op2) ? op1 : op2;
}

template<typename T>
inline T max(T op1, T op2)
{
    return (op1 > op2) ? op1 : op2;
}

template<typename oT, typename iT>
inline oT map(iT val, iT inMin, iT inMax, oT outMin, oT outMax)
{
    float percentage = float(val - inMin) / (inMax - inMin);
    percentage = std::min(1.0f, std::max(0.0f, percentage));
    return oT((outMax - outMin) * percentage) + outMin;
}