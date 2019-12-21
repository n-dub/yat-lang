#pragma once
#include <iostream>
#include "Utils.h"
#include "ErrorChecking.h"

class UnexpectedToken : public Error
{
public:
    UnexpectedToken(uint64_t line, uint64_t c, const String& info)
        : Error(L"Unexpected token:\n" + info + L"\nAt " + std::to_wstring(line)
            + L" : " + std::to_wstring(c) + L"\n")
    {
    }
};

