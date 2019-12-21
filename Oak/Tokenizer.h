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

