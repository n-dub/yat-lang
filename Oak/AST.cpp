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

    if (GetTypeSize(lr) < GetTypeSize(rr))
    {
        return rr;
    }

    return lr;
}

ASTNode* BinOp::TryEval()
{
    ConstLeaf* le = (ConstLeaf*)l->TryEval();
    if (!le) return nullptr;
    ConstLeaf* re = (ConstLeaf*)r->TryEval();
    if (!re) return nullptr;

    switch (oper.type)
    {
        // TODO: constant evaluation
        case TokenType::OperPlus:
        case TokenType::OperMin:
        case TokenType::OperMul:
        case TokenType::OperPow:
        case TokenType::OperDiv:
        case TokenType::OperPCent:
            break;
    }

    return nullptr;
}

bool BinOp::isConstEval()
{
    return l->isConstEval() && r->isConstEval();
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
    return false;
}

bool VarLeaf::isConstEval()
{
    return !data->mut;
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
    return false;
}

bool ConstLeaf::isConstEval()
{
    return true;
}

int64_t ConstLeaf::GetNumberS()
{
    return StringToInt<int64_t>(data.data);
}

uint64_t ConstLeaf::GetNumberU()
{
    return StringToInt<uint64_t>(data.data);
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
    return false;
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
    return false;
}

bool UnOp::isConstEval()
{
    return operand->isConstEval();
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
    return false;
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
    return false;
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
}

Keyword Var::GetTypeKW()
{
    return var_type;
}

ASTNode* Var::TryEval()
{
    return false;
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
    std::wcout << (flags & BType::LeftInclusive ? L"(Inclusive) " : L"(Exclusive) ");

    std::wcout << L"AND\n";

    r->DebugPrint(d + 1);
    AddTabs(d);
    std::wcout << (flags & BType::RightInclusive ? L"(Inclusive)" : L"(Exclusive)") << L"\n";
}

Keyword Range::GetTypeKW()
{
    return Keyword::kw_rng;
}

ASTNode* Range::TryEval()
{
    return nullptr;
}

size_t Range::GetSize()
{
    int64_t res = r->GetNumberS() - l->GetNumberS();

    if (flags & BType::LeftInclusive)
    {
        --res;
    }
    if (flags & BType::RightInclusive)
    {
        --res;
    }

    return res;
}

bool ASTNode::isConstEval()
{
    return false;
}

