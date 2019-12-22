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

BinOp::BinOp()
{
    type = NodeType::BinOper;
}

VarLeaf::VarLeaf(Var* data)
{
    type = NodeType::VarLeaf;
    this->data = data;
}

ConstLeaf::ConstLeaf(const Token& data)
{
    type = NodeType::ConstLeaf;
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

Lambda::Lambda()
{
    type = NodeType::Func;
}

FnCall::FnCall()
{
    type = NodeType::Call;
}

Var::Var()
{
    type = NodeType::Var;
}

