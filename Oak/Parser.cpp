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
#include <sstream>
#include "Parser.h"
#include "ErrorChecking.h"

#define MATCH(_ttype, _msg) cur_tok = m_tok->Next();            \
if (cur_tok.type != TokenType:: ##_ttype) {                     \
    m_tok->UnexpToken(_msg);                                    \
}

#define MATCH_CUR(_ttype, _msg)                                 \
if (cur_tok.type != TokenType:: ##_ttype) {                     \
    m_tok->UnexpToken(_msg);                                    \
} 

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

                MATCH(LBrace, L"Expected a statement block in braces, that defines the namespace");
                cur_tok = m_tok->Next();
                m_nspace->block = ParseBlock(false);
                if (cur_tok.type != TokenType::EoF)
                {
                    cur_tok = m_tok->Next();
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
    ast.prog.push_back(m_nspace);
}

Var* Parser::GetVariable(const String& v)
{
    for (auto& vec : m_vars)
    {
        for (Var* vv : vec)
        {
            if (vv->name == v)
            {
                return vv;
            }
        }
    }

    std::wstringstream ss;
    ss << L"Usage of undeclared variable " << v;
    m_tok->UnexpToken(ss.str());
}

inline void Parser::AddVariable(Var* var)
{
    for (auto& vec : m_vars)
    {
        for (Var* vv : vec)
        {
            if (vv->name == var->name)
            {
                std::wstringstream ss;
                ss << L"Variable " << var->name << (var->mut ? L"mut " : L"")
                    << "has already been defined";
                m_tok->UnexpToken(L"");
            }
        }
    }

    m_vars[m_vars.size() - 1].push_back(var);
}

StatementBlock* Parser::ParseBlock(bool is_fn)
{
    StatementBlock* r = new StatementBlock();
    m_vars.push_back({});

    while (cur_tok.type != TokenType::RBrace
        && cur_tok.type != TokenType::EoF)
    {
        switch (cur_tok.type)
        {
        case TokenType::PPBegin:
            cur_tok = m_tok->Next();
            ParsePreProc();
            break;
        case TokenType::Keyword:
            if (cur_tok.kw_type == Keyword::kw_ret)
            {
                UnOp* ret = new UnOp();
                ret->oper = cur_tok;

                cur_tok = m_tok->Next();

                Keyword expr_type;
                ret->operand = ParseExpression(false, expr_type);
                // TODO: check if the type is valid (maybe not right here)

                r->children.push_back(ret);

                cur_tok = m_tok->Next();
                break;
            }
            else if (cur_tok.kw_type == Keyword::kw_asm)
            {
                if (m_pp.type != PPDir::unsafe)
                {
                    m_tok->UnexpToken(L"Inline assembly is not allowed in a block not marked as unsafe");
                }

                cur_tok = m_tok->Next();

                if (cur_tok.type == TokenType::LParen)
                {
                    cur_tok = m_tok->Next(); // remove '('
                    while (cur_tok.type != TokenType::RParen)
                    {
                        // TODO: parse inline asm parameters
                        cur_tok = m_tok->Next();
                    }
                    cur_tok = m_tok->Next(); // remove ')'
                }

                if (cur_tok.type != TokenType::LBrace)
                {
                    m_tok->UnexpToken(L"Invalid inline assembly syntax");
                }
                cur_tok = m_tok->Next();

                UnOp* res = new UnOp();
                res->oper = Token(L"_asm", TokenType::Keyword, Keyword::kw_asm);
                cur_tok = m_tok->ParseRawUntil(L'}');
                res->operand = new ConstLeaf(cur_tok);
                r->children.push_back(res);

                cur_tok = m_tok->Next();

                break;
            }
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
        if (cur_tok.type == TokenType::Semi)
        {
            cur_tok = m_tok->Next();
        }
    }

    m_pp.Reset();
    m_vars.pop_back();
    r->is_fn = is_fn;
    //cur_tok = m_tok->Next();
    return r;
}

std::vector<ASTNode*> Parser::ParseStatement()
{
    switch (cur_tok.type)
    {
    case TokenType::Keyword: // if current token is a keyword, the statement is variable declaration
    {
        Var* vd = ParseVarDecl();

        if (cur_tok.type == TokenType::Semi)
        {
            //cur_tok = m_tok->Next();
            if (vd->var_type == Keyword::kw_let)
            {
                m_tok->UnexpToken(L"Variable declared with 'let' keyword must be initialized");
            }
            return { vd };
        }

        BinOp* op = new BinOp();
        op->oper = cur_tok;

        if (op->oper.type != TokenType::Assign)
        {
            m_tok->UnexpToken(L"Only assignment (=) is allowed for initialization.");
        }
        op->l = new VarLeaf(vd);
        Keyword right_type;
        cur_tok = m_tok->Next();
        op->r = ParseExpression(vd->var_type == Keyword::kw_fn, right_type);

        if (vd->var_type == Keyword::kw_let)
        {
            vd->var_type = right_type;
        }

        return { vd, op };
    }
    case TokenType::Name:
    case TokenType::OperInc:
    case TokenType::OperDec:
    {
        Keyword q;

        return { ParseExpression(false, q) };
    }
    }

    return { nullptr };
}

ASTNode* Parser::ParseExpression(bool fn, Keyword& exp_type)
{
    if (fn) // parse lambda
    {
        // (i32 p1, string p2) -> ch16 { ret p2[p1]; }
        // or
        // (i32 p1, string p2) -> ch16 ret p2[p1]
        // or
        // i32 p1 -> i32 { ret p1**2; }
        // or
        // mut i32& p1 -> null p1 **= 2

        Lambda* r = new Lambda();
        r->params = ParseParamList();

        m_vars.push_back(r->params); // push all parameters

        StatementBlock* sb;
        MATCH_CUR(Arrow, L"Expected function return type after arrow '->'.");
        cur_tok = m_tok->Next();

        Keyword ret_type = cur_tok.kw_type;

        cur_tok = m_tok->Next();
        if (cur_tok.type == TokenType::LBrace)
        {
            cur_tok = m_tok->Next();
            sb = ParseBlock(true);
        }
        else
        {
            // only single statements are allowed in lambdas without braces {}
            auto st = ParseStatement();
            sb = new StatementBlock();
            for (ASTNode* s : st)
            {
                sb->children.push_back(s);
            }
        }
        m_vars.pop_back();

        r->def = sb;
        r->ret_type = ret_type;
        return r;
    }
    // otherwise parse constant, operation, function call or conditional

    if (cur_tok.kw_type == Keyword::kw_rng) // range expression
    {
        cur_tok = m_tok->Next();
        exp_type = Keyword::kw_rng;
        return ParseRange();
    }
    
    ASTNode* node = ShuntingYard();

    exp_type = node->GetTypeKW();

    return node;
}

inline std::vector<Var*> Parser::ParseParamList()
{
    // only one parameter is allowed without parentheses
    if (cur_tok.type != TokenType::LParen)
    {
        return { ParseVarDecl() };
    }

    cur_tok = m_tok->Next();
    if (cur_tok.type == TokenType::RParen) // empty param list
    {
        cur_tok = m_tok->Next();
        return {};
    }

    std::vector<Var*> r;

    while (true)
    {
        r.push_back(ParseVarDecl());
        if (cur_tok.type == TokenType::RParen) break;
        MATCH(Comma, L"Expected a comma ',' after parameter in lambda");
    }

    cur_tok = m_tok->Next();
    return r;
}

Var* Parser::ParseVarDecl()
{
    Keyword var_type = Keyword::Last;
    String name;
    bool mut = false, is_arr = false;
    Range* array_rgn = nullptr;

    // i32 a = 10;  // stop on '='  (TokenType::Assign)
    // i32 a;       // stop on ';'  (TokenType::Semi)
    // i32 a, ...   // stop on ','  (TokenType::Comma) used in param list
    // i32 a -> ... // stop on '->' (TokenType::Arrow) used in param list
    while (cur_tok.type != TokenType::Assign
        && cur_tok.type != TokenType::Semi
        && cur_tok.type != TokenType::Comma
        && cur_tok.type != TokenType::Arrow)
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
            case Keyword::kw_bool:
            case Keyword::kw_f32:
            case Keyword::kw_f64:
            case Keyword::kw_fn:
            case Keyword::kw_str16:
            case Keyword::kw_let:
            {
                if (var_type != Keyword::Last)
                {
                    m_tok->UnexpToken(L"Multiple data types not allowed.");
                }
                var_type = cur_tok.kw_type;
                cur_tok = m_tok->Next();
                if (cur_tok.type == TokenType::LBracket) // array
                {
                    is_arr = true;
                    cur_tok = m_tok->Next();
                    if (cur_tok.type == TokenType::RBrace)
                    {
                        cur_tok = m_tok->Next();
                    }
                    else
                    {
                        Keyword t;
                        ASTNode* range = ParseExpression(false, t);
                        
                        if (range->type == NodeType::Range)
                        {
                            array_rgn = (Range*)range;
                            cur_tok = m_tok->Next();
                        }
                        else
                        {
                            array_rgn = new Range(
                                new ConstLeaf(
                                    Token(L"0", TokenType::Uint64L)
                                ),
                                (ConstLeaf*)range,
                                Range::LeftInclusive
                            );
                        }
                    }
                }
                continue;
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
                if (cur_tok.data == L"main")
                {
                    name = L"_main";
                }
                else
                {
                    name = cur_tok.data;
                }
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
    r->arr = array_rgn;
    r->is_arr = is_arr;

    AddVariable(r);

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
        cur_tok = m_tok->Next();
    }

    cur_tok = m_tok->Next();
    return r;
}

inline void Parser::ParsePreProc()
{
    while (cur_tok.type != TokenType::PPEnd)
    {
        // TODO: add directives to some list
        if (cur_tok.data == L"unsafe")
        {
            m_pp.type = PPDir::unsafe;
        }
        cur_tok = m_tok->Next();
    }
    //cur_tok = m_tok->Next();
}

inline Range* Parser::ParseRange()
{
    Range* res = new Range();
    if (cur_tok.type == TokenType::LBracket)
    {
        res->flags |= Range::BType::LeftInclusive;
    }
    cur_tok = m_tok->Next();

    Keyword k;
    ASTNode* lb = ParseExpression(false, k);
    //cur_tok = m_tok->Next();

    if (!lb->isConstEval())
    {
        m_tok->UnexpToken(L"Only number constants are allowed in range expressions");
    }

    if (cur_tok.type != TokenType::Semi)
    {
        m_tok->UnexpToken(L"Invalid range expression (expected a semicolon)");
    }

    cur_tok = m_tok->Next();

    ASTNode* rb = ParseExpression(false, k);

    //cur_tok = m_tok->Next();

    if (!lb->isConstEval())
    {
        m_tok->UnexpToken(L"Only number constants are allowed in range expressions");
    }

    if (cur_tok.type == TokenType::RBracket)
    {
        res->flags |= Range::BType::RightInclusive;
    }
    cur_tok = m_tok->Next();

    res->l = (ConstLeaf*)lb;
    res->r = (ConstLeaf*)rb;

    return res;
}

inline ASTNode* Parser::ShuntingYard()
{
    size_t LBrackets = 0;
    size_t LParens = 0;
    // Operator's token : Is operator unary
    std::vector<std::pair<Token, bool>> operatorStack;
    std::vector<ASTNode*> exprStack;

    bool lastNumVar = true; // last token was a number or a variable
    while (cur_tok.type != TokenType::Semi)
    {
        if (cur_tok.type == TokenType::LParen ||
            cur_tok.type == TokenType::LBracket)
        {
            operatorStack.push_back(std::make_pair(cur_tok, true));
            lastNumVar = false;
            if (cur_tok.type == TokenType::LBracket)
            {
                ++LBrackets;
            }
            else
            {
                ++LParens;
            }
        }
        else if (IsNumber(cur_tok.type))
        {
            exprStack.push_back(new ConstLeaf(cur_tok));
            lastNumVar = true;
        }
        else if (cur_tok.type == TokenType::Name)
        {
            Var* var = GetVariable(cur_tok.data);
            exprStack.push_back(new VarLeaf(var));
            lastNumVar = true;
        }
        else if (cur_tok.type == TokenType::RParen)
        {
            if (LParens <= 0) break;
            std::pair<Token, bool> top_op = operatorStack[operatorStack.size() - 1];

            while (top_op.first.type != TokenType::LParen)
            {
                ASTNode* e2 = exprStack[exprStack.size() - 1];
                exprStack.pop_back();

                if (!top_op.second) // operator is binary
                {
                    ASTNode* e1 = exprStack[exprStack.size() - 1];
                    exprStack.pop_back();

                    BinOp* node = new BinOp();
                    node->oper = top_op.first;

                    if (!GetPrecedence(top_op.first.type, top_op.second).right)
                    {
                        node->l = e1;
                        node->r = e2;
                    }
                    else
                    {
                        node->r = e1;
                        node->l = e2;
                    }

                    exprStack.push_back(node);
                }
                else
                {
                    UnOp* node = new UnOp();
                    node->oper = top_op.first;
                    node->operand = e2;

                    exprStack.push_back(node);
                }
                operatorStack.pop_back();
                top_op = operatorStack[operatorStack.size() - 1];
            }

            operatorStack.pop_back();
            lastNumVar = true;
            --LParens;
        }
        else if (cur_tok.type == TokenType::RBracket)
        {
            if (LBrackets <= 0) break;
            std::pair<Token, bool> top_op = operatorStack[operatorStack.size() - 1];

            while (top_op.first.type != TokenType::LBracket)
            {
                ASTNode* e2 = exprStack[exprStack.size() - 1];
                exprStack.pop_back();

                if (!top_op.second) // operator is binary
                {
                    ASTNode* e1 = exprStack[exprStack.size() - 1];
                    exprStack.pop_back();

                    BinOp* node = new BinOp();
                    node->oper = top_op.first;

                    if (!GetPrecedence(top_op.first.type, top_op.second).right)
                    {
                        node->l = e1;
                        node->r = e2;
                    }
                    else
                    {
                        node->r = e1;
                        node->l = e2;
                    }

                    exprStack.push_back(node);
                }
                else
                {
                    UnOp* node = new UnOp();
                    node->oper = top_op.first;
                    node->operand = e2;

                    exprStack.push_back(node);
                }
                operatorStack.pop_back();
                top_op = operatorStack[operatorStack.size() - 1];
            }

            operatorStack.pop_back();
            lastNumVar = true;
            --LBrackets;
        }
        else // operator
        {
            if (operatorStack.size())
            {
                OperPrec topPrec = GetPrecedence(operatorStack[operatorStack.size() - 1].first.type,
                    operatorStack[operatorStack.size() - 1].second);

                OperPrec curPrec = GetPrecedence(cur_tok.type, !lastNumVar);

                while (topPrec >= curPrec)
                {
                    std::pair<Token, bool> top_op = operatorStack[operatorStack.size() - 1];

                    ASTNode* e2 = exprStack[exprStack.size() - 1];
                    exprStack.pop_back();

                    if (!top_op.second) // operator is binary
                    {
                        ASTNode* e1 = exprStack[exprStack.size() - 1];
                        exprStack.pop_back();

                        BinOp* node = new BinOp();
                        node->oper = top_op.first;

                        if (!topPrec.right)
                        {
                            node->l = e1;
                            node->r = e2;
                        }
                        else
                        {
                            node->r = e1;
                            node->l = e2;
                        }

                        exprStack.push_back(node);
                    }
                    else
                    {
                        UnOp* node = new UnOp();
                        node->oper = top_op.first;
                        node->operand = e2;

                        exprStack.push_back(node);
                    }
                    operatorStack.pop_back();
                    topPrec = GetPrecedence(operatorStack[operatorStack.size() - 1].first.type,
                        operatorStack[operatorStack.size() - 1].second);
                }
            }
            operatorStack.push_back(std::make_pair(cur_tok, !lastNumVar));

            lastNumVar = false;
        }
        cur_tok = m_tok->Next();
    }

    if (operatorStack.size() <= 0)
    {
        return exprStack[exprStack.size() - 1];
    }

    std::pair<Token, bool> top_op = operatorStack[operatorStack.size() - 1];
    while (true)
    {
        ASTNode* e2 = exprStack[exprStack.size() - 1];
        exprStack.pop_back();

        if (!top_op.second) // operator is binary
        {
            ASTNode* e1 = exprStack[exprStack.size() - 1];
            exprStack.pop_back();

            BinOp* node = new BinOp();
            node->oper = top_op.first;

            if (!GetPrecedence(top_op.first.type, top_op.second).right)
            {
                node->l = e1;
                node->r = e2;
            }
            else
            {
                node->r = e1;
                node->l = e2;
            }

            exprStack.push_back(node);
        }
        else
        {
            UnOp* node = new UnOp();
            node->oper = top_op.first;
            node->operand = e2;

            exprStack.push_back(node);
        }
        operatorStack.pop_back();
        if (operatorStack.size() <= 0)
        {
            break;
        }
        top_op = operatorStack[operatorStack.size() - 1];
    }



    return exprStack[exprStack.size() - 1];
}

