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
    // leaf node (a constant or a variable)
    Leaf,
    // any keyword
    Keyword,
    // statement block
    StBlock
};

class ASTNode
{
public:
    NodeType type = NodeType::None;
};

class Leaf : public ASTNode
{
public:
    Token data;
    Leaf(const Token& data);
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
    StatementBlock* def = nullptr;
    std::vector<Token> params;
    Token retType;
};

class FnCall : public ASTNode
{
public:
    Token FnName; // is needed before linking stage
    Lambda* func = nullptr;
    std::vector<ASTNode*> params;
};

class Namespace
{
public:
    String name;
    // all using statements
    std::vector<String> uses;
    // block of all statements of the namespace
    StatementBlock block;
};

class AST
{
public:
    // The whole program
    // Vector contains all namespaces
    std::vector<Namespace*> prog;
};

