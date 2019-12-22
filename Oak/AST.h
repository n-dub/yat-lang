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
    Var
};

class ASTNode
{
public:
    NodeType type = NodeType::None;
};

class ConstLeaf : public ASTNode
{
public:
    Token data;
    ConstLeaf(const Token& data);
};

// Unary Operator representation
class UnOp : public ASTNode
{
public:
    ASTNode* operand = nullptr;
    Token oper;
    UnOp();
};

// Binary Operator representation
class BinOp : public ASTNode
{
public:
    ASTNode* l = nullptr;
    ASTNode* r = nullptr;
    Token oper;
    BinOp();
};

class StatementBlock : public ASTNode
{
public:
    StatementBlock();
    std::vector<ASTNode*> children;
};

class Lambda : public ASTNode
{
public:
    Lambda();
    StatementBlock* def = nullptr;
    std::vector<Token> params;
    Token retType;
};

class FnCall : public ASTNode
{
public:
    FnCall();
    Token FnName; // is needed before linking stage
    Lambda* func = nullptr;
    std::vector<ASTNode*> params;
};

// variable declaration
class Var : public ASTNode
{
public:
    Var();
    String name;
    Keyword var_type;
    bool mut = false;
};

// using value of a variable in a statement
class VarLeaf : public ASTNode
{
public:
    Var* data;
    VarLeaf(Var* data);
};

class Namespace
{
public:
    String name;
    // all using statements
    std::vector<String> uses;
    // block of all statements of the namespace
    StatementBlock* block;
};

class AST
{
public:
    // The whole program
    // Vector contains all namespaces
    std::vector<Namespace*> prog;
};

