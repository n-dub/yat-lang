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
#include "Compiler.h"
#include "Tokenizer.h"
#include <chrono>
#include "Parser.h"

Compiler::Compiler(const String& inp, const String& outp)
{
    m_input = inp;
    m_output = outp;
}

bool Compiler::Run()
{
    try
    {
        Tokenizer tok(m_input);

        auto st = std::chrono::high_resolution_clock::now();

        Parser parser(tok);
        AST tree;
        parser.Parse(tree);

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

