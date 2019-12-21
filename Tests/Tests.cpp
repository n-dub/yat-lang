#include <iostream>
#include "../Oak/Tokenizer.h"

int main()
{
    String s = LR"(
name name2 = 123i64 + 64u8 - 128 % 12 -= rty;
)";
    Tokenizer parser(s, false);

    Token t(L"", TokenType::Assign);

    while (t.type != TokenType::EoF)
    {
        t = parser.Next();
        std::wcout << (int)t.type << " " << t.data << "\n";
    }

    return 0;
}

