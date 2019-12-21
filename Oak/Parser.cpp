#include "Parser.h"
#include "ErrorChecking.h"
Parser::Parser(Tokenizer& t)
{
    m_tok = &t;
}

void Parser::Parse(AST& ast)
{
    Token t = m_tok->Next();

    while (t.type != TokenType::EoF)
    {
        Namespace* ns = new Namespace();

        if (t.type == TokenType::Keyword)
        {
            if (t.kw_type == Keyword::kw_nspace)
            {
                Token name = m_tok->Next();
                if (name.type == TokenType::Name)
                {
                    ns->name = name.data;
                }
                else
                {
                    m_tok->UnexpToken(L"Expected name of namespace");
                }
            }
            else if (t.kw_type == Keyword::kw_using)
            {
                Token name = m_tok->Next();
                if (name.type == TokenType::Name)
                {
                    ns->uses.push_back(name.data);
                }
                else
                {
                    m_tok->UnexpToken(L"Expected name of namespace");
                }
            }
        }
        else
        {
            m_tok->UnexpToken(L"Program in Oak consists of namespaces, so only either "
                L"'nspace' or 'using' keywords are allowed outsize of namespace.");
        }
    }
}

