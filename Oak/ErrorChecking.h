//  Oak programming language
//  Copyright (C) 2019  Nikita Dubovikov
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
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

