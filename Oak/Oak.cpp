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
#include <iostream>
#include <vector>
#include <string>
#include "Compiler.h"

constexpr const char* HelpMsg = R"(
Usage:
    oak <input files> [options]

Options:
    -h                                  show this help message
    -o <path>                           output file (without extension)
)";

int main(int argc, char** argv)
{
    std::wstring output, input;

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

            S2W(args[i], input);
        }
    }

    Compiler comp(input, output);
    if (comp.Run())
    {
        return 0;
    }
    else
    {
        std::wcerr << comp.GetError();
        std::wcout << "Build failed";
        return 1;
    }
}

