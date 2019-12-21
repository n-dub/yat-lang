#pragma once
#include "Tokenizer.h"
#include "AST.h"

class Parser
{
    Tokenizer* m_tok = nullptr;
public:
    Parser(Tokenizer& t);
    void Parse(AST& ast);
};

