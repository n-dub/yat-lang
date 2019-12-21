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

