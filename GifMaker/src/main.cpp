#include <iostream>
#include <fstream>
#include <string>
#include "scriptanimator.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
        std::cout << "usage: " << argv[0] << " script.js\n";
    else
    {
        std::ifstream file(argv[1]);

        if (file.is_open())
        {
            std::string source(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            ScriptAnimator(source.c_str()).Execute();
        }
        else
            std::cout << "bad input file\n";
    }
    return 0;
}
