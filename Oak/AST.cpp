#include "AST.h"

BinOp::BinOp()
{
    type = NodeType::BinOper;
}

Leaf::Leaf(const Token& data)
{
    type = NodeType::Leaf;
    this->data = data;
}

StatementBlock::StatementBlock()
{
    type = NodeType::StBlock;
}

UnOp::UnOp()
{
    type = NodeType::UnOper;
}

