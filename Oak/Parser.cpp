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
#include "Parser.h"
#include "ErrorChecking.h"
Parser::Parser(Tokenizer& t)
{
    m_tok = &t;
}

void Parser::Parse(AST& ast)
{
    cur_tok = m_tok->Next();

    while (cur_tok.type != TokenType::EoF)
    {
        m_nspace = new Namespace();

        if (cur_tok.type == TokenType::Keyword)
        {
            if (cur_tok.kw_type == Keyword::kw_nspace)
            {
                cur_tok = m_tok->Next();
                if (cur_tok.type == TokenType::Name)
                {
                    m_nspace->name = cur_tok.data;
                }
                else
                {
                    m_tok->UnexpToken(L"Expected name of namespace");
                }

                if (m_tok->Next().type == TokenType::LBrace)
                {
                    m_nspace->block = ParseBlock();
                }
                else
                {
                    m_tok->UnexpToken(L"Expected a statement block in braces, that defines the namespace");
                }
            }
            else if (cur_tok.kw_type == Keyword::kw_using)
            {
                m_nspace->uses.push_back(ParseUsing());
            }
            else
            {
                m_tok->UnexpToken(L"Program in Oak consists of namespaces, so only either "
                    L"'nspace' or 'using' keywords are allowed outside of namespace.");
            }
        }
        else
        {
            m_tok->UnexpToken(L"Program in Oak consists of namespaces, so only either "
                L"'nspace' or 'using' keywords are allowed outside of namespace.");
        }
    }
}

StatementBlock* Parser::ParseBlock()
{
    StatementBlock* r = new StatementBlock();

    while (cur_tok.type != TokenType::RBrace)
    {
        switch (cur_tok.type)
        {
        case TokenType::Keyword:
            // TODO: check for keywords: for, while, if, do, etc. (but not type names)
            // and parse them in another function
        case TokenType::Name:
        case TokenType::OperInc:
        case TokenType::OperDec:
        {
            // return vector because a statement can consist of multiple nodes
            auto st = ParseStatement();
            for (ASTNode* s : st)
            {
                r->children.push_back(s);
            }

            break;
        }
        case TokenType::EoF:
        {
            m_tok->UnexpToken(L"End of file while parsing statement block. Did you forget to close the brace?");
        }
        default:
        {
            m_tok->UnexpToken(L"A statement must begin with either a type name, a declared variable name or a prefix operator.");
        }
        }
        cur_tok = m_tok->Next();
    }

    return r;
}

std::vector<ASTNode*> Parser::ParseStatement()
{
    ASTNode* r = nullptr;
    switch (cur_tok.type)
    {
    case TokenType::Keyword: // if current token is a keyword, the statement is variable declaration
    {
        Var* vd = ParseVarDecl();

        if (cur_tok.type == TokenType::Semi)
        {
            cur_tok = m_tok->Next();
            return { vd };
        }

        BinOp* op = new BinOp();
        op->oper = cur_tok;

        if (op->oper.type != TokenType::Assign)
        {
            m_tok->UnexpToken(L"Only assignment (=) is allowed for initialization.");
        }
        op->l = new VarLeaf(vd);
        op->r = ParseExpression();

        return { vd, op };
    }
    }

    return { r };
}

ASTNode* Parser::ParseExpression()
{

}

Var* Parser::ParseVarDecl()
{
    Keyword var_type = Keyword::Last;
    String name;
    bool mut = false;

    // i32 a = 10; // stop on '=' (TokenType::Assign)
    // i32 a;      // stop on ';' (TokenType::Semi)
    while (cur_tok.type != TokenType::Assign && cur_tok.type != TokenType::Semi)
    {
        if (cur_tok.type == TokenType::Keyword)
        {
            switch (cur_tok.kw_type)
            {
                // all types
            case Keyword::kw_u8:
            case Keyword::kw_u16:
            case Keyword::kw_u32:
            case Keyword::kw_u64:
            case Keyword::kw_i8:
            case Keyword::kw_i16:
            case Keyword::kw_i32:
            case Keyword::kw_i64:
            case Keyword::kw_ch16:
            case Keyword::kw_ch32:
            case Keyword::kw_bool:
            case Keyword::kw_f32:
            case Keyword::kw_f64:
            case Keyword::kw_fn:
            case Keyword::kw_str16:
            case Keyword::kw_let: // for 'let' keyword the type will be detected later
            {
                if (var_type != Keyword::Last)
                {
                    m_tok->UnexpToken(L"Multiple data types not allowed.");
                }
                var_type = cur_tok.kw_type;
                break;
            }
            case Keyword::kw_mut: // mutable
            {
                mut = true;
                break;
            }
            default:
                m_tok->UnexpToken(L"Invalid keyword in statement.");
            }
        }
        else
        {
            if (cur_tok.type == TokenType::Name)
            {
                name = cur_tok.data;
            }
            else
            {
                m_tok->UnexpToken(L"Invalid statement. Expected name of the variable.");
            }
        }
        cur_tok = m_tok->Next();
    }

    Var* r = new Var();
    r->mut = mut;
    r->var_type = var_type;
    r->name = name;

    return r;
}

inline String Parser::ParseUsing()
{
    cur_tok = m_tok->Next();
    String r = L"";

    while (cur_tok.type != TokenType::EoF && cur_tok.type != TokenType::Semi)
    {
        if (cur_tok.type == TokenType::Name || cur_tok.type == TokenType::Dot)
        {
            r += cur_tok.data;
        }
        else
        {
            m_tok->UnexpToken(L"Invalid token in using statement or missing semicolon.");
        }
    }

    return r;
}

