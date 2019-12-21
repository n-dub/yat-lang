#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>

using String = std::wstring;

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
inline T StringToInt(const String& str)
{
    T buf = 0;
    for (int i = 0; i < str.length(); ++i)
    {
        buf *= 10;
        buf += str[i] - L'0';
    }
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

inline bool ReadFile(const String& path, String& buffer)
{
    std::wifstream wif(path);
    if (!wif.is_open()) return false;

    wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();
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

