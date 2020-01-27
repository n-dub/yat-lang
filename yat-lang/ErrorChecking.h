//  Yat programming language
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
    inline String GenString(const String& str, uint64_t cs, uint64_t ce)
    {
        if (cs == ce)
        {
            cs = 0;
            ce = str.length();
        }

        String res;
        res.reserve(ce - cs);
        for (int i = 0; i < ce; ++i)
        {
            if (i >= cs)
            {
                res += L'~';
            }
            else if (str[i] == L'\t')
            {
                res += L"    ";
            }
            else
            {
                res += L" ";
            }
        }

        res += L"\n\n";
        return res;
    }

public:
    UnexpectedToken(uint64_t line,
        const String& info,
        const String& code_line,
        uint64_t cs,
        uint64_t ce,
        const String& file)
        : Error(L"\nUnexpected token in file `" + file + L"':\n" + info + L"\nAt line #" + std::to_wstring(line)
            + L"\n\n" + code_line + L"\n" + GenString(code_line, cs, ce))
    {
    }
};

