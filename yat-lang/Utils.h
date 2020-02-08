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
#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>

using String = std::wstring;

inline void S2W(const std::string& narrow, std::wstring& wide);

// Get system environment variable
inline String GetEnvVar(const std::string& key)
{
    char* val = getenv(key.c_str());

    if (val)
    {
        String wres;
        S2W(val, wres);
        return wres;
    }

    return String(L"");
}

class Error
{
    String m_msg;
public:
    Error(const String& msg)
    {
        m_msg = msg;
    }

    const String& what() const
    {
        return m_msg;
    }
};

template<class T>
inline T _StringToNumHelper(const String& str)
{
    T buf = 0;
    int i = 0;

    bool neg = str[0] == L'-';
    if (neg) ++i;

    for (; i < str.length(); ++i)
    {
        buf *= 10;
        buf += str[i] - L'0';
    }

    if (neg && std::is_signed<T>()) buf *= -1;

    return buf;
}

template<class T>
inline T StringToNum(const String& str)
{
    // As I tested this algorithm is 4-5 times faster than std::stoi (stof, stod, etc.)
    // for float and up to 9 times faster for long double and long long. (g++ -Ofast)

    if constexpr(std::is_integral<T>()) return _StringToNumHelper<T>(str);

    T buf = 0;
    int i = 0;

    bool neg = str[0] == L'-';
    if (neg) ++i;

    for (; i < str.length(); ++i)
    {
        if (str[i] == L'.')
        {
            ++i;
            String f = str.substr(i); // string with all digits after point
            T n = (T)_StringToNumHelper<uint64_t>(f);
            // if str = "123.45"
            // buf before '.' is 123
            // n will be 45
            //              -2
            // buf + 45 * 10  = 123 + 0.45 = 123.45
            buf += n / std::pow(10, f.length());
            break;
        }
        buf *= 10;
        buf += str[i] - L'0';
    }

    if (neg && std::is_signed<T>()) buf *= -1;

    return buf;
}

template<class T>
inline String IntToString(T n)
{
    String str;
    while (n)
    {
        str = static_cast<wchar_t>(n % 10 + L'0') + str;
        n /= 10;
    }
    if (str == L"") return L"0";
    return str;
}

inline void S2W(const std::string& narrow, std::wstring& wide)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    wide = converter.from_bytes(narrow);
}

// add file contents to the buffer
inline bool ReadFile(const String& path, String& buffer)
{
    std::wifstream wif(path);
    if (!wif.is_open()) return false;

    auto _ = wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << buffer << wif.rdbuf();
    buffer = wss.str();

    return true;
}

inline bool IsAlpha(wchar_t c)
{
    return iswalpha(c);
}

inline bool IsAlpha(char c)
{
    return isalpha(c);
}

inline bool IsNumber(wchar_t c)
{
    return iswdigit(c);
}

inline bool IsNumber(char c)
{
    return isdigit(c);
}

