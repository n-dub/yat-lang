#include "Compiler.h"
#include "Tokenizer.h"
#include <chrono>

Compiler::Compiler(const String& inp, const String& outp)
{
    m_input = inp;
    m_output = outp;
}

bool Compiler::Run()
{
    try
    {
        Tokenizer parser(m_input);

        Token t = parser.Next();

        auto st = std::chrono::high_resolution_clock::now();

        while (t.type != TokenType::EoF)
        {
            std::wcout << (int)t.type << " " << t.data << "\n";
            t = parser.Next();
        }

        auto sec = (std::chrono::high_resolution_clock::now() - st);

        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(sec).count();
    }
    catch(const Error& ex)
    {
        m_error = ex.what();
        return false;
    }

    return true;
}

String Compiler::GetError()
{
    return m_error;
}

