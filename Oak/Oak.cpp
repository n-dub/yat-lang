#include <iostream>
#include <vector>
#include <string>

constexpr const char* HelpMsg = "";

int main(int argc, char** argv)
{
    std::string output, input;
    if (argc > 1)
    {
        std::vector<std::string> args;
        args.resize(argc - 1ull);

        for (int i = 0; i < args.size(); ++i)
        {
            args[i] = argv[i];
        }

        for (int i = 0; i < args.size(); ++i)
        {
            if (args[0] == "-h")
            {
                std::cout << HelpMsg;
                return 0;
            }

            if (i > 0 && args[i - 1ull] == "-o")
            {
                output = args[i];
                continue;
            }

            input = args[i];
        }
    }
    else
    {
        std::cout << HelpMsg;
    }

    return 0;
}

