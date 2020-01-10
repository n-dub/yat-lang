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
#pragma once
#include "Tokens.h"
#include <vector>

enum class NodeType
{
    None = 0,
    // binary operation
    BinOper,
    // unary operation
    UnOper,
    // function call
    Call,
    // function definition
    Func,
    // leaf node (constant)
    ConstLeaf,
    // leaf node (variable)
    VarLeaf,
    // statement block
    StBlock,
    // variable declaration
    Var,
    // range expression
    Range,
    // string literal (str16)
    String,
    // type conversion
    Cvt,
    // is-statement
    IfSt,
    // while-loop
    WhileLoop
};

class ASTNode
{
public:
    NodeType type = NodeType::None;
    virtual bool isConstEval();
    virtual void DebugPrint(size_t d) = 0;
    virtual ASTNode* TryEval() = 0;
    virtual Keyword GetTypeKW() = 0;
    virtual void AddTypeCvt() = 0;
};

class Convert : public ASTNode
{
public:
    ASTNode* value{};
    Convert();
    Convert(Keyword t);
    Convert(ASTNode* v, Keyword t);
    Keyword to{};
    virtual bool isConstEval();
    virtual void DebugPrint(size_t d);
    virtual ASTNode* TryEval();
    virtual Keyword GetTypeKW();
    virtual void AddTypeCvt();
};

class ConstLeaf;

class Range : public ASTNode
{
public:
    Range();
    Range(ConstLeaf* start, ConstLeaf* end, uint8_t flags);
    ConstLeaf *l, *r;

    enum
    {
        LeftInclusive  = 0b01,
        RightInclusive = 0b10
    };
    uint8_t flags = 0;

    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    size_t GetSize();
    virtual void AddTypeCvt();
};

class ConstLeaf : public ASTNode
{
public:
    Token data;
    ConstLeaf(const Token& data);
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    int64_t GetNumberS();
    uint64_t GetNumberU();
    virtual void AddTypeCvt();
};

class StrLeaf : public ASTNode
{
public:
    Token data;
    StrLeaf(const Token& data);
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

// Unary Operator representation
class UnOp : public ASTNode
{
public:
    ASTNode* operand = nullptr;
    Token oper;
    UnOp();
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

// Binary Operator representation
class BinOp : public ASTNode
{
public:
    ASTNode* l = nullptr;
    ASTNode* r = nullptr;
    Token oper;
    BinOp();
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

class StatementBlock : public ASTNode
{
public:
    StatementBlock();
    // size of all local variables in bytes
    size_t bytes = 0;
    std::vector<ASTNode*> children;
    bool is_fn = false;
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual void AddTypeCvt();
};

class Var;

class Lambda : public ASTNode
{
public:
    Lambda();
    StatementBlock* def = nullptr;
    std::vector<Var*> params;
    Keyword ret_type{};
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual void AddTypeCvt();
};

class FnCall : public ASTNode
{
public:
    FnCall();
    Token FnName; // is needed before linking stage
    Lambda* func = nullptr;
    std::vector<ASTNode*> params;
    virtual void DebugPrint(size_t d) override;
    // TODO: func may be nullptr
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual void AddTypeCvt();
};

// variable declaration
class Var : public ASTNode
{
public:
    Var();
    Var(String n, Keyword t, bool m = false, Range* a = nullptr);
    String name;
    Keyword var_type = Keyword::Last;
    bool mut = false, is_arr = false, is_param = false;
    Range* arr = nullptr;
    ASTNode* initial = nullptr;

    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual void AddTypeCvt();
};

// using value of a variable in a statement
class VarLeaf : public ASTNode
{
public:
    Var* data{};
    VarLeaf(Var* data);
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

// if-statement
class IfStatement : public ASTNode
{
public:
    ASTNode* condition{};
    StatementBlock* then_b{};
    StatementBlock* else_b{};
    IfStatement();
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

// while-loop
class WhileLoop : public ASTNode
{
public:
    ASTNode* condition{};
    StatementBlock* body{};
    WhileLoop();
    virtual void DebugPrint(size_t d) override;
    virtual Keyword GetTypeKW() override;
    virtual ASTNode* TryEval() override;
    virtual bool isConstEval();
    virtual void AddTypeCvt();
};

class Namespace
{
public:
    String name;
    // all using statements
    std::vector<String> uses;
    // block of all statements of the namespace
    StatementBlock* block;
    void DebugPrint(size_t d);
};

class AST
{
public:
    // The whole program
    // Vector contains all namespaces
    std::vector<Namespace*> prog;
    void DebugPrint();
};

