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
#include "Tokenizer.h"
#include "ErrorChecking.h"

bool EofReturned = false;

wchar_t Tokenizer::GetChar()
{
    if (m_code[m_offset] == '\n')
    {
        ++line;
        col = 1;
    }
    else
    {
        ++col;
    }

    return m_code[m_offset++];
}

void Tokenizer::EatChars(size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        GetChar();
    }
}

void Tokenizer::SkipWhite()
{
    wchar_t c = m_code[m_offset];
    while (iswspace(c) || iswblank(c) || c == L'\t' || c == 0)
    {
        GetChar();
        if (m_offset == m_code.size()) break;
        c = m_code[m_offset];
    }
}

// check if given number token is valid, i.e. specified number of bits is enough
bool Tokenizer::IsNumValid(const Token& t)
{
    switch (t.type)
    {
    case TokenType::Int8L:
        return IntToString(StringToNum<int8_t>(t.data)) == t.data;
    case TokenType::Int16L:
        return IntToString(StringToNum<int16_t>(t.data)) == t.data;
    case TokenType::Int32L:
        return IntToString(StringToNum<int32_t>(t.data)) == t.data;
    case TokenType::Int64L:
        return IntToString(StringToNum<int64_t>(t.data)) == t.data;

    case TokenType::Uint8L:
        return IntToString(StringToNum<uint8_t>(t.data)) == t.data;
    case TokenType::Uint16L:
        return IntToString(StringToNum<uint16_t>(t.data)) == t.data;
    case TokenType::Uint32L:
        return IntToString(StringToNum<uint32_t>(t.data)) == t.data;
    case TokenType::Uint64L:
        return IntToString(StringToNum<uint64_t>(t.data)) == t.data;

    case TokenType::Float32L:
        return IntToString(StringToNum<float>(t.data)) == t.data;
    case TokenType::Float64L:
        return IntToString(StringToNum<double>(t.data)) == t.data;

    default:
        return false;
    }
}

Keyword Tokenizer::IsKeyword(const String& str)
{
    for (int i = 0; i < (int)Keyword::Last; ++i)
    {
        if (str == KeywordStr[i])
        {
            return (Keyword)i;
        }
    }

    return Keyword::Last;
}

Tokenizer::Tokenizer(const std::vector<String>& str, bool path)
{
    EofReturned = false;
    m_code = L"";
    if (path)
    {
        uint64_t st_line = 0;
        for (const String& s : str)
        {
            if (!ReadFile(s, m_code))
            {
                throw Error(L"file " + s + L" not found\n");
            }

            uint64_t lines = std::count(m_code.cbegin(), m_code.cend(), L'\n');
            files.push_back(FileInfo(st_line, st_line + lines, s));
            st_line += lines;
        }
    }
    else
    {
        for (const String& s : str)
        {
            m_code += s;
        }
    }
    m_code += L" \n \n "; // adding a bit of garbage to prevent overflow
    m_offset = 0;
}

void Tokenizer::UnexpToken(const String& msg)
{
    size_t _0{};
    String r = GetLine(m_offset, m_offset, _0);
    throw UnexpectedToken(prevLine - files[m_cfile].StartLine, msg, r, 0, 0, files[m_cfile].Name);
}

void Tokenizer::UnexpToken(const String& msg, Token* t)
{
    size_t s{};
    String r = GetLine(t->start, t->end, s);
    throw UnexpectedToken(t->line - files[m_cfile].StartLine, msg, r, t->start - s, t->end - s, files[m_cfile].Name);
}

Token Tokenizer::ParseName()
{
    String r = L"";

    uint64_t s_offset = m_offset;

    wchar_t c = m_code[m_offset];

    while (IsAlpha(c) || IsNumber(c) || c == L'_')
    {
        r += c;
        GetChar();
        c = m_code[m_offset];
    }

    Keyword kw = IsKeyword(r);

    auto t = Token(r, TokenType::Name, s_offset, m_offset, line, kw);
    SkipWhite();
    return t;
}

String Tokenizer::ParseNumber()
{
    String r = L"";

    wchar_t c = m_code[m_offset];

    while (IsNumber(c) || c == L'_' || c == L'.')
    {
        if (c != L'_') r += c;
        GetChar();
        c = m_code[m_offset];
    }

    return r;
}

Token Tokenizer::ParseNumberLiteral()
{
    String r = ParseNumber();
    auto s_offset = m_offset;

    if (m_code[m_offset] == L'i')
    {
        GetChar();
        String bits = ParseNumber();
        if (bits == L"8")
        {
            auto t = Token(r, TokenType::Int8L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"16")
        {
            auto t = Token(r, TokenType::Int16L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"32")
        {
            auto t = Token(r, TokenType::Int32L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"64")
        {
            auto t = Token(r, TokenType::Int64L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        UnexpToken(L"\"i\" after a number literal must be followed by number of bits (8, 16, 32 or 64).");
    }

    if (m_code[m_offset] == L'u')
    {
        ++m_offset;
        String bits = ParseNumber();
        if (bits == L"8")
        {
            auto t = Token(r, TokenType::Uint8L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"16")
        {
            auto t = Token(r, TokenType::Uint16L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"32")
        {
            auto t = Token(r, TokenType::Uint32L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"64")
        {
            auto t = Token(r, TokenType::Uint64L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        UnexpToken(L"\"u\" after a number literal must be followed by number of bits (8, 16, 32 or 64).");
    }
    if (m_code[m_offset] == L'f')
    {
        ++m_offset;
        String bits = ParseNumber();
        if (bits == L"32")
        {
            auto t = Token(r, TokenType::Float32L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        if (bits == L"64")
        {
            auto t = Token(r, TokenType::Float64L, s_offset, m_offset, line);
            SkipWhite();
            return t;
        }
        UnexpToken(L"\"f\" after a floating-point number literal must be followed by number of bits (32 or 64).");
    }
    auto t = Token(r, 
        r.find_first_of(L'.') == String::npos ? TokenType::Int32L : TokenType::Float64L,
        s_offset, m_offset, line);
    SkipWhite();
    return t;
}

wchar_t Tokenizer::ParseEscapeChar()
{
    switch (m_code[m_offset])
    {
        // punctuation
    case L'\\':
        return L'\\';
    case L'\'':
        return L'\'';
    case L'"':
        return L'"';

        // control characters
    case L'r':
        return L'\r';
    case L'f':
        return L'\f';
    case L'v':
        return L'\v';
    case L'n':
        return L'\n';
    case L't':
        return L'\t';
    case L'b':
        return L'\b';
    case L'a':
        return L'\a';
    case L'0':
        return L'\0';
    }
    UnexpToken(L"Invalid escape character. If you need to use a backslash (\\), double it (\\\\) to avoid this error.");
}

String Tokenizer::GetLine(uint64_t offs, uint64_t offe, size_t& s)
{
    s = m_code.find_last_of(L'\n', offs);
    if (s == String::npos) s = 0;
    size_t e = m_code.find_first_of(L'\n', offe);

    String res = m_code.substr(s, e - s);

    int i = 0;
    for (; i < res.length(); ++i)
    {
        if (!iswspace(res[i]))
        {
            break;
        }
    }

    s += i;

    return res.substr(i);
}

void Tokenizer::NextLine()
{
    while (m_code[m_offset] != L'\n')
    {
        GetChar();
    }
    GetChar();
}

void Tokenizer::SkipComments()
{
    while (true)
    {
        if (m_code[m_offset] == L'/')
        {
            if (m_code[m_offset + 1] == L'/')
            {
                while (m_code[m_offset] == L'/' && m_code[m_offset + 1] == L'/')
                {
                    NextLine();
                    SkipWhite();
                }
            }
            else if (m_code[m_offset + 1] == L'*')
            {
                GetChar(); // skip /
                GetChar(); // skip *
                while (true)
                {
                    if (m_code[m_offset] == L'*' && m_code[m_offset + 1] == L'/')
                    {
                        GetChar(); // skip *
                        GetChar(); // skip /
                        break;
                    }
                    GetChar();
                }
            }
            else
            {
                SkipWhite();
                return;
            }
        }
        else
        {
            SkipWhite();
            return;
        }
    }
}

template<>
Token Tokenizer::ParseStringLiteral<Tokenizer::StringType::Regular>()
{
    String r = L"";
    auto s_offset = m_offset;

    while (m_offset < m_code.length())
    {
        switch (m_code[m_offset])
        {
        case L'"':
        {
            GetChar();
            return Token(r, TokenType::String, s_offset, m_offset, line);
        }
        case L'\n':
        {
            UnexpToken(L"End of line while parsing a string literal. Did you forget to close a quote (\")?");
        }
        case L'\\': // escape character
        {
            GetChar();
            r += ParseEscapeChar();
            GetChar();
            continue;
        }
        }

        r += m_code[m_offset];
        GetChar();
    }
    UnexpToken(L"End of file while parsing a string literal. Did you forget to close a quote (\")?");
}

template<>
Token Tokenizer::ParseStringLiteral<Tokenizer::StringType::Raw>()
{
    String r = L"";
    auto s_offset = m_offset;

    while (m_offset < m_code.length())
    {
        if (m_code[m_offset] == L')' && m_code[m_offset + 1] == L'"')
        {
            GetChar();
            GetChar();
            return Token(r, TokenType::String, s_offset, m_offset, line);
        }

        r += m_code[m_offset];
        GetChar();
    }

    UnexpToken(L"End of file while parsing a raw string literal. Did you forget to close a quote with bracket?");
}

Token Tokenizer::ParseRawUntil(wchar_t tt)
{
    auto s_offset = m_offset;
    size_t off = m_code.find_first_of(tt, m_offset) - m_offset;
    String str = m_code.substr(m_offset - 2, off);
    EatChars(off);
    prevCol = col;
    prevLine = line;
    prevOffset = m_offset;
    return Token(str, TokenType::String, s_offset, m_offset, line);
}

Token Tokenizer::ParseOperator()
{
    auto s_offset = m_offset;
    switch (m_code[m_offset])
    {
    case L'+':
        GetChar();
        if (m_code[m_offset] == L'+')
        {
            GetChar();
            return Token(L"++", TokenType::OperInc, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"+=", TokenType::AssignPlus, s_offset, m_offset, line);
        }
        return Token(L"+", TokenType::OperPlus, s_offset, m_offset, line);

    case L'-':
        GetChar();
        if (m_code[m_offset] == L'-')
        {
            GetChar();
            return Token(L"--", TokenType::OperDec, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"-=", TokenType::AssignMin, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'>')
        {
            GetChar();
            return Token(L"->", TokenType::Arrow, s_offset, m_offset, line);
        }
        return Token(L"-", TokenType::OperMin, s_offset, m_offset, line);

    case L'*':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"*=", TokenType::AssignMul, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'*')
        {
            GetChar();
            if (m_code[m_offset] == L'=')
            {
                GetChar();
                return Token(L"**=", TokenType::AssignPow, s_offset, m_offset, line);
            }
            return Token(L"**", TokenType::OperPow, s_offset, m_offset, line);
        }
        return Token(L"*", TokenType::OperMul, s_offset, m_offset, line);
    case L'/':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"/=", TokenType::AssignDiv, s_offset, m_offset, line);
        }
        return Token(L"/", TokenType::OperDiv, s_offset, m_offset, line);
    case L'%':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"%=", TokenType::AssignPCent, s_offset, m_offset, line);
        }
        return Token(L"%", TokenType::OperPCent, s_offset, m_offset, line);
    case L'^':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"^=", TokenType::AssignXor, s_offset, m_offset, line);
        }
        return Token(L"^", TokenType::OperXor, s_offset, m_offset, line);
    case L'<':
        GetChar();
        if (m_code[m_offset] == L'<')
        {
            GetChar();
            if (m_code[m_offset] == L'=')
            {
                GetChar();
                return Token(L"<<=", TokenType::AssignLShift, s_offset, m_offset, line);
            }
            return Token(L"<<", TokenType::OperLShift, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"<=", TokenType::OperLEqual, s_offset, m_offset, line);
        }
        return Token(L"<", TokenType::OperLess, s_offset, m_offset, line);
    case L'>':
        GetChar();
        if (m_code[m_offset] == L'>')
        {
            GetChar();
            if (m_code[m_offset] == L'=')
            {
                GetChar();
                return Token(L">>=", TokenType::AssignRShift, s_offset, m_offset, line);
            }
            return Token(L">>", TokenType::OperRShift, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L">=", TokenType::OperGEqual, s_offset, m_offset, line);
        }
        return Token(L">", TokenType::OperGreater, s_offset, m_offset, line);
    case L'&':
        GetChar();
        if (m_code[m_offset] == L'&')
        {
            GetChar();
            return Token(L"&&", TokenType::OperLAnd, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"&=", TokenType::AssignBWAnd, s_offset, m_offset, line);
        }
        return Token(L"&", TokenType::OperBWAnd, s_offset, m_offset, line);
    case L'|':
        GetChar();
        if (m_code[m_offset] == L'|')
        {
            GetChar();
            return Token(L"||", TokenType::OperLOr, s_offset, m_offset, line);
        }
        else if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"|=", TokenType::AssignBWOr, s_offset, m_offset, line);
        }
        return Token(L"|", TokenType::OperBWOr, s_offset, m_offset, line);
    case L'~':
        GetChar();
        return Token(L"~", TokenType::OperNot, s_offset, m_offset, line);
    case L'!':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"!=", TokenType::OperNEqual, s_offset, m_offset, line);
        }
        return Token(L"!", TokenType::OperLNot, s_offset, m_offset, line);
    case L';':
        GetChar();
        return Token(L";", TokenType::Semi, s_offset, m_offset, line);
    case L'=':
        GetChar();
        if (m_code[m_offset] == L'=')
        {
            GetChar();
            return Token(L"==", TokenType::OperEqual, s_offset, m_offset, line);
        }
        return Token(L"=", TokenType::Assign, s_offset, m_offset, line);

    case L'{':
        GetChar();
        return Token(L"{", TokenType::LBrace, s_offset, m_offset, line);
    case L'}':
        GetChar();
        return Token(L"}", TokenType::RBrace, s_offset, m_offset, line);

    case L'(':
        GetChar();
        return Token(L"(", TokenType::LParen, s_offset, m_offset, line);
    case L')':
        GetChar();
        if (m_code[m_offset] == L'!')
        {
            GetChar();
            return Token(L")!", TokenType::PPEnd, s_offset, m_offset, line);
        }
        return Token(L")", TokenType::RParen, s_offset, m_offset, line);

    case L'[':
        GetChar();
        return Token(L"[", TokenType::LBracket, s_offset, m_offset, line);
    case L']':
        GetChar();
        return Token(L"]", TokenType::RBracket, s_offset, m_offset, line);

    case L'.':
        GetChar();
        return Token(L".", TokenType::Dot, s_offset, m_offset, line);
    case L',':
        GetChar();
        return Token(L",", TokenType::Comma, s_offset, m_offset, line);
    case L':':
        GetChar();
        return Token(L":", TokenType::Colon, s_offset, m_offset, line);
    case L'?':
        GetChar();
        return Token(L"?", TokenType::Quest, s_offset, m_offset, line);
        // #!( PREPROCESSOR )!
    case L'#':
        GetChar();
        if (m_code[m_offset] == L'!')
        {
            GetChar();
            if (m_code[m_offset == '('])
            {
                GetChar();
                return Token(L"#!(", TokenType::PPBegin, s_offset, m_offset, line);
            }
        }
        UnexpToken(L"Invalid preprocessor directive.");

    default:
        GetChar();
        UnexpToken(L"Expected an operator. Unknown character " + m_code[m_offset]);
    }
}

Token Tokenizer::Next()
{
    if (SkipNext)
    {
        SkipNext = false;
        return SkipToken;
    }

    if (m_offset >= m_code.size())
    {
        if (EofReturned) UnexpToken(L"End of file");
        EofReturned = true;
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;

        return Token(L"EoF", TokenType::EoF, m_offset, m_offset, line);
    }

    SkipWhite();

    if (m_offset >= m_code.size())
    {
        if (EofReturned) UnexpToken(L"End of file");
        EofReturned = true;
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;

        return Token(L"EoF", TokenType::EoF, m_offset, m_offset, line);
    }

    SkipComments();

    if (m_offset >= m_code.size())
    {
        if (EofReturned) UnexpToken(L"End of file");
        EofReturned = true;
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;

        return Token(L"EoF", TokenType::EoF, m_offset, m_offset, line);
    }

    wchar_t s = m_code[m_offset];
    wchar_t ns = m_code[m_offset + 1];

    // "Regular string"
    if (s == L'"')
    {
        GetChar(); // remove "
        auto r = ParseStringLiteral<StringType::Regular>();
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;
        if (line > files[m_cfile].EndLine)
        {
            ++m_cfile;
        }
        return r;
    }

    // @"(Raw string)"
    if (s == L'@')
    {
        if (ns == L'"' && m_code[m_offset + 2] == L'(')
        {
            GetChar(); // remove @
            GetChar(); // remove "
            GetChar(); // remove (
            auto r = ParseStringLiteral<StringType::Raw>();
            prevCol = col;
            prevLine = line;
            prevOffset = m_offset;
            if (line > files[m_cfile].EndLine)
            {
                ++m_cfile;
            }
            return r;
        }
        else
        {
            UnexpToken(L"Invalid syntax for Raw string. It must look like this: @\"(Raw string)\"");
        }
    }

    if (IsAlpha(s) || s == L'_')
    {
        auto r = ParseName();
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;
        if (line > files[m_cfile].EndLine)
        {
            ++m_cfile;
        }
        return r;
    }

    if (IsNumber(s))
    {
        auto r = ParseNumberLiteral();
        if (!IsNumValid(r))
        {
            throw Error(L"Invalid number literal. It doesn't fit the given number of bits.\n");
        }
        SkipWhite();
        prevCol = col;
        prevLine = line;
        prevOffset = m_offset;
        if (line > files[m_cfile].EndLine)
        {
            ++m_cfile;
        }
        return r;
    }

    auto r = ParseOperator();
    SkipWhite();
    prevCol = col;
    prevLine = line;
    prevOffset = m_offset;
    if (line > files[m_cfile].EndLine)
    {
        ++m_cfile;
    }
    return r;
}

Tokenizer::FileInfo::FileInfo(uint64_t s, uint64_t e, const String& n)
{
    StartLine = s;
    EndLine = e;
    Name = n;
}

