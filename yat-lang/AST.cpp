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
#include "AST.h"
#include <iostream>

void AddTabs(size_t n)
{
    for (int i = 0; i < n; ++i)
    {
        std::wcout << L"|   ";
    }
}

BinOp::BinOp()
{
    type = NodeType::BinOper;
}

void BinOp::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Binary operator " << oper.data << L" Operands:\n";
    l->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << L"AND\n";
    r->DebugPrint(d + 1);
}

Keyword BinOp::GetTypeKW()
{
    Keyword lr = l->GetTypeKW(), rr = r->GetTypeKW();

    switch (oper.type)
    {
    case TokenType::OperLAnd:
    case TokenType::OperLOr:
    case TokenType::OperLNot:
    case TokenType::OperLess:
    case TokenType::OperGreater:
    case TokenType::OperEqual:
    case TokenType::OperNEqual:
    case TokenType::OperLEqual:
    case TokenType::OperGEqual:
    {
        return Keyword::kw_bool;
    }
    }

    if (GetTypeSize(lr) < GetTypeSize(rr))
    {
        return rr;
    }

    return lr;
}

ASTNode* BinOp::TryEval()
{
    l = l->TryEval();
    r = r->TryEval();

    if (l->type != NodeType::ConstLeaf || r->type != NodeType::ConstLeaf)
    {
        return this;
    }

    auto ln = ((ConstLeaf*)l)->GetNumber();
    auto rn = ((ConstLeaf*)r)->GetNumber();
    auto t = GetTypeKW();

#define OPER_CASE(_op)\
    switch (t)\
    {\
    case Keyword::kw_i8:  return new ConstLeaf(Token(std::to_wstring(ln.ib _op rn.ib), TokenType::Int8L));\
    case Keyword::kw_u8:  return new ConstLeaf(Token(std::to_wstring(ln.ub _op rn.ub), TokenType::Uint8L));\
    case Keyword::kw_i16: return new ConstLeaf(Token(std::to_wstring(ln.iw _op rn.iw), TokenType::Int16L));\
    case Keyword::kw_u16: return new ConstLeaf(Token(std::to_wstring(ln.uw _op rn.uw), TokenType::Uint16L));\
    case Keyword::kw_i32: return new ConstLeaf(Token(std::to_wstring(ln.id _op rn.id), TokenType::Int32L));\
    case Keyword::kw_u32: return new ConstLeaf(Token(std::to_wstring(ln.ud _op rn.ud), TokenType::Uint32L));\
    case Keyword::kw_i64: return new ConstLeaf(Token(std::to_wstring(ln.iq _op rn.iq), TokenType::Int64L));\
    case Keyword::kw_u64: return new ConstLeaf(Token(std::to_wstring(ln.uq _op rn.uq), TokenType::Uint64L));\
    }

    switch (oper.type)
    {
        case TokenType::OperPlus:  OPER_CASE(+);
        case TokenType::OperMin:   OPER_CASE(-);
        case TokenType::OperMul:   OPER_CASE(*);
        case TokenType::OperPow:
        {
            switch (t)
            {
            case Keyword::kw_i8:  return new ConstLeaf(Token(std::to_wstring(std::pow(ln.ib, rn.ib)), TokenType::Int8L));
            case Keyword::kw_u8:  return new ConstLeaf(Token(std::to_wstring(std::pow(ln.ub, rn.ub)), TokenType::Uint8L));
            case Keyword::kw_i16: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.iw, rn.iw)), TokenType::Int16L));
            case Keyword::kw_u16: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.uw, rn.uw)), TokenType::Uint16L));
            case Keyword::kw_i32: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.id, rn.id)), TokenType::Int32L));
            case Keyword::kw_u32: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.ud, rn.ud)), TokenType::Uint32L));
            case Keyword::kw_i64: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.iq, rn.iq)), TokenType::Int64L));
            case Keyword::kw_u64: return new ConstLeaf(Token(std::to_wstring(std::pow(ln.uq, rn.uq)), TokenType::Uint64L));
            }
        }
        case TokenType::OperDiv:   OPER_CASE(/);
        case TokenType::OperPCent: OPER_CASE(*);
            break;
    }

    return this;
}

bool BinOp::isConstEval()
{
    return l->isConstEval() && r->isConstEval();
}

void BinOp::AddTypeCvt()
{
    l->AddTypeCvt();
    r->AddTypeCvt();
    size_t sl = GetTypeSize(l->GetTypeKW()), sr = GetTypeSize(r->GetTypeKW());

    if (sl < sr)
    {
        r = new Convert(r, l->GetTypeKW());
    }
    else if (sl > sr)
    {
        l = new Convert(l, r->GetTypeKW());
    }
}

VarLeaf::VarLeaf(Var* data)
{
    type = NodeType::VarLeaf;
    this->data = data;
}

void VarLeaf::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Variable " << (data->mut ? L"mut " : L"")
        << KeywordStr[(int)data->var_type] << L" " << data->name << L"\n";
}

Keyword VarLeaf::GetTypeKW()
{
    return data->GetTypeKW();
}

ASTNode* VarLeaf::TryEval()
{
    return this;
}

bool VarLeaf::isConstEval()
{
    return !data->mut;
}

void VarLeaf::AddTypeCvt()
{
}

ConstLeaf::ConstLeaf(const Token& data)
{
    type = NodeType::ConstLeaf;
    this->data = data;
}

void ConstLeaf::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Constant: " << data.data << L"\n";
}

Keyword ConstLeaf::GetTypeKW()
{
    return TTypeToKeyword(data.type);
}

ASTNode* ConstLeaf::TryEval()
{
    return this;
}

bool ConstLeaf::isConstEval()
{
    return true;
}

ConstLeaf::GetNumRes ConstLeaf::GetNumber()
{
    GetNumRes res{};

    switch (GetTypeKW())
    {
    case Keyword::kw_i8: res.ib = StringToInt<int8_t>(data.data);
    case Keyword::kw_u8: res.ub = StringToInt<uint8_t>(data.data);
    case Keyword::kw_i16: res.iw = StringToInt<int16_t>(data.data);
    case Keyword::kw_u16: res.uw = StringToInt<uint16_t>(data.data);
    case Keyword::kw_i32: res.id = StringToInt<int32_t>(data.data);
    case Keyword::kw_u32: res.ud = StringToInt<uint32_t>(data.data);
    case Keyword::kw_i64: res.iq = StringToInt<int64_t>(data.data);
    case Keyword::kw_u64: res.uq = StringToInt<uint64_t>(data.data);
    }

    return res;
}

void ConstLeaf::AddTypeCvt()
{
}

StatementBlock::StatementBlock()
{
    type = NodeType::StBlock;
}

void StatementBlock::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Block of statements:\n";
    for (ASTNode* node : children)
    {
        node->DebugPrint(d + 1);
    }
}

Keyword StatementBlock::GetTypeKW()
{
    return Keyword::Last;
}

ASTNode* StatementBlock::TryEval()
{
    return this;
}

void StatementBlock::AddTypeCvt()
{
    for (ASTNode* node : children)
    {
        node->AddTypeCvt();
    }
}

UnOp::UnOp()
{
    type = NodeType::UnOper;
}

void UnOp::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Unary operator " << oper.data << L" Operand:\n";
    operand->DebugPrint(d + 1);
}

Keyword UnOp::GetTypeKW()
{
    return operand->GetTypeKW();
}

ASTNode* UnOp::TryEval()
{
    operand = operand->TryEval();
    if (oper.type == TokenType::OperMin)
    {
        if (operand->type == NodeType::ConstLeaf)
        {
            ConstLeaf* v = (ConstLeaf*)operand;
            if (!IsSigned(v->GetTypeKW()))
            {
                throw Error(L"Unary operator '-' applied to unsigned type\n");
            }

            if (v->data.data[0] == L'-')
            {
                v->data.data = v->data.data.substr(1);
            }
            else
            {
                v->data.data = L'-' + v->data.data;
            }
            return v;
        }
    }
    return this;
}

bool UnOp::isConstEval()
{
    return operand->isConstEval();
}

void UnOp::AddTypeCvt()
{
    operand->AddTypeCvt();
}

Lambda::Lambda()
{
    type = NodeType::Func;
}

void Lambda::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Lambda " << KeywordStr[(int)ret_type] << L" Parameters:\n";

    for (Var* p : params)
    {
        p->DebugPrint(d + 1);
    }

    AddTabs(d);
    std::wcout << L"Block:\n";

    def->DebugPrint(d + 1);
}

Keyword Lambda::GetTypeKW()
{
    return Keyword::kw_fn;
}

ASTNode* Lambda::TryEval()
{
    return this;
}

void Lambda::AddTypeCvt()
{
    def->AddTypeCvt();
}

FnCall::FnCall()
{
    type = NodeType::Call;
}

void FnCall::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Calling function " << FnName.data << L" with params:\n";
    for (ASTNode* n : params)
    {
        n->DebugPrint(d + 1);
    }
}

Keyword FnCall::GetTypeKW()
{
    return func->ret_type;
}

ASTNode* FnCall::TryEval()
{
    return this;
}

void FnCall::AddTypeCvt()
{
    for (int i = 0; i < params.size(); ++i)
    {
        if (params[i]->GetTypeKW() != func->params[i]->GetTypeKW())
        {
            params[i] = new Convert(params[i], func->params[i]->GetTypeKW());
        }
    }
}

Var::Var()
{
    type = NodeType::Var;
}

Var::Var(String n, Keyword t, bool m, Range* a)
{
    type = NodeType::Var;
    name = n;
    var_type = t;
    mut = m;
    arr = a;
}

void Var::DebugPrint(size_t d)
{
    AddTabs(d);
    if (!is_arr)
    {
        std::wcout << L"Declaration of variable " << (mut ? L"mut " : L"")
            << KeywordStr[(int)var_type] << L" " << name << L"\n";
    }
    else
    {
        std::wcout << L"Declaration of array " << (mut ? L"mut " : L"")
            << KeywordStr[(int)var_type] << L" " << name << L" in range:" << L"\n";
        arr->DebugPrint(d + 1);
    }
    if (initial)
    {
        AddTabs(d);
        std::wcout << L"Initialized with:\n";
        initial->DebugPrint(d + 1);
    }
}

Keyword Var::GetTypeKW()
{
    return var_type;
}

ASTNode* Var::TryEval()
{
    return this;
}

void Var::AddTypeCvt()
{
    if (!initial) return;

    initial->AddTypeCvt();

    if (GetTypeSize(initial->GetTypeKW()) != GetTypeSize(var_type))
    {
        initial = new Convert(initial, var_type);
    }
}

void AST::DebugPrint()
{
    std::wcout << L"Printing AST namespaces:\n";

    for (Namespace* ns : prog)
    {
        ns->DebugPrint(1);
    }
}

void Namespace::DebugPrint(size_t d)
{
    AddTabs(d);

    std::wcout << L"Namespace " << name << L"\n";

    block->DebugPrint(d + 1);
}

Range::Range()
{
    type = NodeType::Range;
    l = r = nullptr;
}

Range::Range(ConstLeaf* start, ConstLeaf* end, uint8_t flags)
{
    type = NodeType::Range;
    l = start;
    r = end;
    this->flags = flags;
}

void Range::DebugPrint(size_t d)
{
    AddTabs(d);

    std::wcout << L"Range expression between\n";

    l->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << (flags & LeftInclusive ? L"(Inclusive) " : L"(Exclusive) ");

    std::wcout << L"AND\n";

    r->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << (flags & RightInclusive ? L"(Inclusive)" : L"(Exclusive)") << L"\n";
}

Keyword Range::GetTypeKW()
{
    return Keyword::kw_rng;
}

ASTNode* Range::TryEval()
{
    return this;
}

size_t Range::GetSize()
{
    int64_t res = r->GetNumber().iq - l->GetNumber().iq;

    if (flags & LeftInclusive)
    {
        --res;
    }
    if (flags & RightInclusive)
    {
        --res;
    }

    return res;
}

void Range::AddTypeCvt()
{
}

bool ASTNode::isConstEval()
{
    return false;
}

StrLeaf::StrLeaf(const Token& data)
{
    type = NodeType::String;
    this->data = data;
}

void StrLeaf::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"String literal: " << data.data << L"\n";
}

Keyword StrLeaf::GetTypeKW()
{
    return Keyword::kw_str16;
}

ASTNode* StrLeaf::TryEval()
{
    return this;
}

bool StrLeaf::isConstEval()
{
    return false;
}

void StrLeaf::AddTypeCvt()
{
}

IfStatement::IfStatement()
{
    type = NodeType::IfSt;
}

void IfStatement::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"If:\n";
    condition->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << L"Then:\n";
    then_b->DebugPrint(d + 1);
    if (else_b)
    {
        AddTabs(d);
        std::wcout << L"Else:\n";
        else_b->DebugPrint(d + 1);
    }
}

Keyword IfStatement::GetTypeKW()
{
    return Keyword::Last;
}

ASTNode* IfStatement::TryEval()
{
    return this;
}

bool IfStatement::isConstEval()
{
    return false;
}

void IfStatement::AddTypeCvt()
{
    condition->AddTypeCvt();
    then_b->AddTypeCvt();
    if (else_b)
    {
        else_b->AddTypeCvt();
    }
}

WhileLoop::WhileLoop()
{
    type = NodeType::WhileLoop;
}

void WhileLoop::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"WHILE:\n";
    condition->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << L"DO:\n";
    body->DebugPrint(d + 1);
}

Keyword WhileLoop::GetTypeKW()
{
    return Keyword::Last;
}

ASTNode* WhileLoop::TryEval()
{
    return this;
}

bool WhileLoop::isConstEval()
{
    return false;
}

void WhileLoop::AddTypeCvt()
{
    condition->AddTypeCvt();
    body->AddTypeCvt();
}

Convert::Convert()
{
    type = NodeType::Cvt;
}

Convert::Convert(Keyword t)
{
    type = NodeType::Cvt;
    to = t;
}

Convert::Convert(ASTNode* v, Keyword t)
{
    type = NodeType::Cvt;
    to = t;
    value = v;
}

bool Convert::isConstEval()
{
    return value->isConstEval();;
}

void Convert::DebugPrint(size_t d)
{
    AddTabs(d);
    std::wcout << L"Convert to " << KeywordStr[(size_t)to] << L":\n";
    value->DebugPrint(d + 1);
}

ASTNode* Convert::TryEval()
{
    value = value->TryEval();
    return this;
}

Keyword Convert::GetTypeKW()
{
    return to;
}

void Convert::AddTypeCvt()
{
}

