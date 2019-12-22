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
#include <vector>
#include <string>
#include "Utils.h"
#include "Tokens.h"

class Tokenizer
{
    enum class StringType
    {
        Regular,
        Raw
    };

    String m_code;
    uint64_t m_offset = 0, line = 1, col = 1;
    wchar_t GetChar();
    void SkipWhite();
    void NextLine();
    void SkipComments();
    bool IsNumValid(const Token& t);
    Keyword IsKeyword(const String& str);
    wchar_t ParseEscapeChar();
public:
    Tokenizer(const String& str, bool path = true);

    void UnexpToken(const String& msg);

    Token ParseName();
    String ParseNumber();
    Token ParseNumberLiteral();

    template<StringType T>
    Token ParseStringLiteral();

    Token ParseOperator();
    Token Next();
};

