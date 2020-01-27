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
    uint64_t m_offset = 0, line = 1, col = 1, prevLine = 1, prevCol = 1, prevOffset = 0;
    wchar_t GetChar();
    void EatChars(size_t n);
    void SkipWhite();
    void NextLine();
    void SkipComments();
    bool IsNumValid(const Token& t);
    Keyword IsKeyword(const String& str);
    wchar_t ParseEscapeChar();
    String GetLine(uint64_t offs, uint64_t offe, size_t& s);

    // index of the current file
    uint64_t m_cfile = 0;
    void UnexpToken(const String& msg);

public:
    struct FileInfo
    {
        uint64_t StartLine = 0, EndLine = 0;
        String Name{};
        FileInfo(uint64_t s, uint64_t e, const String& n);
    };

    std::vector<FileInfo> files{};

    bool SkipNext = false;
    Token SkipToken{};

    Tokenizer(const std::vector<String>& str, bool path = true);

    void UnexpToken(const String& msg, Token* t);

    Token ParseName();
    String ParseNumber();
    Token ParseNumberLiteral();

    template<StringType T>
    Token ParseStringLiteral();

    // get string containing all characters before char tt
    // and do not remove this char
    Token ParseRawUntil(wchar_t tt);

    Token ParseOperator();
    Token Next();
};

