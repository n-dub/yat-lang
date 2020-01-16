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
#include <iostream>
#include <vector>
#include <string>
#include "Compiler.h"

constexpr const char* HelpMsg = R"(
Usage:
    yat <input files> [options]

Options:
    -h                                  show this help message
    -o <path>                           output file (without extension)
    -S                                  compile file, but do not assemble and link
    -O[0-3]                             level of optimization
)";

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "");
    std::wstring output, input;
    bool assembly = false;
    int opt_level = 0;

    if (argc <= 1)
    {
        std::cout << HelpMsg;
        return 0;
    }

    {
        std::vector<std::string> args;
        args.resize(argc);

        for (int i = 0; i < args.size(); ++i)
        {
            args[i] = argv[i];
        }

        for (int i = 1; i < args.size(); ++i)
        {
            if (args[0] == "-h")
            {
                std::cout << HelpMsg;
                return 0;
            }

            if (args[i] == "-o")
            {
                S2W(args[i + (size_t)1], output);
                ++i;
                continue;
            }

            if (args[i] == "-S")
            {
                assembly = true;
                continue;
            }

            if (args[i] == "-O0")
            {
                opt_level = 0;
                continue;
            }

            if (args[i] == "-O1")
            {
                opt_level = 1;
                continue;
            }

            if (args[i] == "-O2")
            {
                opt_level = 2;
                continue;
            }

            if (args[i] == "-O3")
            {
                opt_level = 3;
                continue;
            }

            if (args[i][0] == '-')
            {
                std::wcout << L"WARNING: unrecognized compiler option `";
                std::cout << args[i];
                std::wcout << L"' ignored\n";
                continue;
            }

            S2W(args[i], input);
        }
    }

    Compiler comp(input, output, assembly);
    if (comp.Run())
    {
        return 0;
    }

    std::wcerr << comp.GetError() << L"\n\nBuild failed\n";
    return 1;
}

