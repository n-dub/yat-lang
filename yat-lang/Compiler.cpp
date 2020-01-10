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
#include <chrono>
#include <filesystem>
#include "Compiler.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "CodeGen.h"

Compiler::Compiler(const String& inp, const String& outp, bool as)
{
    m_input = inp;
    m_output = outp;
    m_as_outp = as;
}

bool Compiler::Run()
{
    try
    {
        Tokenizer* tok = new Tokenizer({ m_input });

        std::vector<String> imports;
        Token t = tok->Next();
        String lib_path = GetEnvVar("YatLibDir");
        if (lib_path == L"")
        {
            std::wcout << L"WARNING: Cannot find Oak standard library.\n"
                << L"Its path doesn't exist in system environment variables.\n";
        }
        else if (lib_path[lib_path.length() - 1] != L'\\')
        {
            lib_path += L"\\";
        }

        while (t.type != TokenType::EoF)
        {
            // parsing imports to include folders from standard library to the project
            if (t.type == TokenType::Name && t.data == L"import")
            {
                t = tok->Next();
                if (t.type == TokenType::String)
                {
                    String path = lib_path + t.data;
                    for (const auto& entry : std::filesystem::directory_iterator(path))
                        imports.push_back(entry.path().wstring());
                }
                else
                {
                    tok->UnexpToken(L"invalid token in import statement\n");
                }
            }
            try
            {
                t = tok->Next();
            }
            catch (...) // catch eny error the tokenizer can throw
            {
            }

            if (t.type == TokenType::Semi)
            {
                t = tok->Next();
            }
        }

        imports.push_back(m_input);

        delete tok;
        tok = new Tokenizer(imports);

        auto st = std::chrono::high_resolution_clock::now();

        Parser parser(tok);
        AST tree;
        parser.Parse(tree);

        // tree.DebugPrint();
        // return true;

        CodeGen cg(tree);
        // if (m_as_outp)
        // {
        cg.WriteCode(m_output + L".s");
        // }
        // else
        // {
        //     cg.WriteCode(m_output + L".s");
        //     String cmd = L"gcc " + m_output + L".s -o " + m_output + L" -m64";
        //     std::wcout << cmd << L"\n";
        //     _wsystem(cmd.c_str());
        // }

        auto sec = (std::chrono::high_resolution_clock::now() - st);
        std::wcout << std::chrono::duration_cast<std::chrono::milliseconds>(sec).count();
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

