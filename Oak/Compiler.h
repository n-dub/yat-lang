#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Utils.h"

class Compiler
{
    String m_output, m_input, m_error;

public:
    Compiler(const String& inp, const String& outp);
    bool Run();
    String GetError();
};

