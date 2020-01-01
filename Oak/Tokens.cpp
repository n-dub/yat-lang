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
#include "Tokens.h"

const wchar_t* MnemonicStr[]{
    L"mov",
    L"add",
    L"sub",
    L"imul",
    L"mul",
    L"idiv",
    L"div",
    L"shl",
    L"shr",
    L"and",
    L"or",
    L"not",
    L"xor",
    L"inc",
    L"dec",
    L"lea"
};

const wchar_t* KeywordStr[]{
    L"using",
    L"nspace",
    L"fn",
    L"i8",
    L"i16",
    L"i32",
    L"i64",
    L"u8",
    L"u16",
    L"u32",
    L"u64",
    L"f32",
    L"f64",
    L"ch16",
    L"str16",
    L"bool",
    L"ret",
    L"mut",
    L"true",
    L"false",
    L"break",
    L"continue",
    L"if",
    L"else",
    L"while",
    L"for",
    L"do",
    L"let",
    L"null",
    L"class",
    L"pub",
    L"prv",
    L"prt",
    L"vrt",
    L"base",
    L"new",
    L"_asm",
    L"rng"
};

OperPrec GetPrecedence(TokenType oper, bool unary)
{
    switch (oper)
    {
    case TokenType::Assign:
    case TokenType::AssignPlus:
    case TokenType::AssignMin:
    case TokenType::AssignMul:
    case TokenType::AssignPow:
    case TokenType::AssignDiv:
    case TokenType::AssignPCent:
    case TokenType::AssignLShift:
    case TokenType::AssignRShift:
    case TokenType::AssignBWAnd:
    case TokenType::AssignBWOr:
    case TokenType::AssignXor:
        return OperPrec(false, 10);

    case TokenType::OperPlus:
        return OperPrec(false, 26);
    case TokenType::OperMin:
        return unary ? OperPrec(true, 29) : OperPrec(false, 26);
    case TokenType::OperMul:
        return OperPrec(false, 27);
    case TokenType::OperPow:
        return OperPrec(true, 28);
    case TokenType::OperDiv:
        return OperPrec(false, 27);
    case TokenType::OperPCent:
        return OperPrec(false, 27);
    case TokenType::OperInc:
    case TokenType::OperDec:
        return unary ? OperPrec(true, 29) : OperPrec(false, 50);
    case TokenType::OperLShift:
        return OperPrec(false, 25);
    case TokenType::OperRShift:
        return OperPrec(false, 25);
    case TokenType::OperBWAnd:
        return OperPrec(false, 22);
    case TokenType::OperBWOr:
        return OperPrec(false, 20);
    case TokenType::OperNot:
        return OperPrec(true, 30);
    case TokenType::OperXor:
        return OperPrec(false, 23);

    case TokenType::OperLess:
        return OperPrec(false, 24);
    case TokenType::OperGreater:
        return OperPrec(false, 24);
    case TokenType::OperEqual:
        return OperPrec(false, 23);
    case TokenType::OperNEqual:
        return OperPrec(false, 23);
    case TokenType::OperLEqual:
        return OperPrec(false, 24);
    case TokenType::OperGEqual:
        return OperPrec(false, 24);

    case TokenType::OperLAnd:
        return unary ? OperPrec(true, 60) : OperPrec(false, 19);
    case TokenType::OperLOr:
        return OperPrec(false, 18);
    case TokenType::OperLNot:
        return OperPrec(true, 30);

    case TokenType::LBracket:
        return OperPrec(false, 0);
    case TokenType::LParen:
        return OperPrec(false, 0);

    case TokenType::Dot:
        return OperPrec(false, 40);
    }
    return OperPrec(false, 0);
}

bool IsNumber(TokenType type)
{
    return (type == TokenType::Uint8L)
        || (type == TokenType::Uint16L)
        || (type == TokenType::Uint32L)
        || (type == TokenType::Uint64L)
        || (type == TokenType::Int8L)
        || (type == TokenType::Int16L)
        || (type == TokenType::Int32L)
        || (type == TokenType::Int64L);
}

bool IsNumber(Keyword kw)
{
    return (kw == Keyword::kw_i8)
        || (kw == Keyword::kw_i16)
        || (kw == Keyword::kw_i32)
        || (kw == Keyword::kw_i64)
        || (kw == Keyword::kw_u8)
        || (kw == Keyword::kw_u16)
        || (kw == Keyword::kw_u32)
        || (kw == Keyword::kw_u64);
}

bool IsBinaryOp(TokenType type)
{
    switch (type)
    {
    case TokenType::Assign:
    case TokenType::AssignPlus:
    case TokenType::AssignMin:
    case TokenType::AssignMul:
    case TokenType::AssignPow:
    case TokenType::AssignDiv:
    case TokenType::AssignPCent:
    case TokenType::AssignLShift:
    case TokenType::AssignRShift:
    case TokenType::AssignBWAnd:
    case TokenType::AssignBWOr:
    case TokenType::AssignXor:
    case TokenType::OperPlus:
    case TokenType::OperMin:
    case TokenType::OperMul:
    case TokenType::OperPow:
    case TokenType::OperDiv:
    case TokenType::OperPCent:
    case TokenType::OperLShift:
    case TokenType::OperRShift:
    case TokenType::OperBWAnd:
    case TokenType::OperBWOr:
    case TokenType::OperNot:
    case TokenType::OperXor:
    case TokenType::OperLess:
    case TokenType::OperGreater:
    case TokenType::OperEqual:
    case TokenType::OperNEqual:
    case TokenType::OperLEqual:
    case TokenType::OperGEqual:
    case TokenType::OperLAnd:
    case TokenType::OperLOr:
    case TokenType::OperLNot:
        return true;
    default:
        return false;
    }
}

Keyword TTypeToKeyword(TokenType t)
{
    switch (t)
    {
    case TokenType::Int8L:
        return Keyword::kw_i8;
    case TokenType::Int16L:
        return Keyword::kw_i16;
    case TokenType::Int32L:
        return Keyword::kw_i32;
    case TokenType::Int64L:
        return Keyword::kw_i64;
    case TokenType::Uint8L:
        return Keyword::kw_u8;
    case TokenType::Uint16L:
        return Keyword::kw_u16;
    case TokenType::Uint32L:
        return Keyword::kw_u32;
    case TokenType::Uint64L:
        return Keyword::kw_u64;
    case TokenType::Float32L:
        return Keyword::kw_f32;
    case TokenType::Float64L:
        return Keyword::kw_f64;
    case TokenType::String:
        return Keyword::kw_str16;
    case TokenType::Char:
        return Keyword::kw_ch16;
    }
    return Keyword::Last;
}

size_t GetTypeSize(Keyword kw)
{
    switch (kw)
    {
    case Keyword::kw_rng:
        return 8 + 8 + 1; // 8 for left and right numbers and 1 for flags
    case Keyword::kw_i8:
    case Keyword::kw_u8:
    case Keyword::kw_bool:
        return 1;
        break;
    case Keyword::kw_i16:
    case Keyword::kw_u16:
    case Keyword::kw_ch16:
        return 2;
        break;
    case Keyword::kw_i32:
    case Keyword::kw_u32:
    case Keyword::kw_f32:
        return 4;
        break;
    case Keyword::kw_i64:
    case Keyword::kw_u64:
    case Keyword::kw_f64:
    case Keyword::kw_fn:
        return 8;
        break;
    }
    return 0;
}

