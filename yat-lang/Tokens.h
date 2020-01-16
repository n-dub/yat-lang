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
#include <string>
#include "Utils.h"

enum class TokenType
{
    // end of file
    EoF,

    // semicolon ";"
    Semi,
    // colon ":"
    Colon,
    // dot "."
    Dot,
    // comma ","
    Comma,

    // question mark "?"
    Quest,

    // arrow "->" operator for lambdas
    Arrow,

    // any name [A-Z|a-z|_]+[A-Z|a-z|0-9|_]*
    Name,
    // any keyword
    Keyword,

    // operator =
    Assign,

    // operator +=
    AssignPlus,
    // operator -=
    AssignMin,
    // operator *=
    AssignMul,
    // operator **=
    AssignPow,
    // operator /=
    AssignDiv,
    // operator %=
    AssignPCent,

    // operator <<=
    AssignLShift,
    // operator >>=
    AssignRShift,
    // operator &=
    AssignBWAnd,
    // operator |=
    AssignBWOr,
    // operator ^=
    AssignXor,

    // operator +
    OperPlus,
    // operator -
    OperMin,
    // operator *
    OperMul,
    // operator **
    OperPow,
    // operator /
    OperDiv,
    // operator %
    OperPCent,
    // operator ++
    OperInc,
    // operator --
    OperDec,

    // Bitwise operators

    // operator <<
    OperLShift,
    // operator >>
    OperRShift,
    // operator &
    OperBWAnd,
    // operator |
    OperBWOr,
    // operator ~
    OperNot,
    // operator ^
    OperXor,

    // operator <
    OperLess,
    // operator >
    OperGreater,
    // operator ==
    OperEqual,
    // operator !=
    OperNEqual,
    // operator <=
    OperLEqual,
    // operator >=
    OperGEqual,

    // Logical operators

    // operator &&
    OperLAnd,
    // operator ||
    OperLOr,
    // operator !
    OperLNot,

    // parentheses

    // left round bracket '('
    LParen,
    // right round bracket ')'
    RParen,

    // braces

    // left curly bracket '{'
    LBrace,
    // right curly bracket '}'
    RBrace,

    // brackets

    // left square bracket '['
    LBracket,
    // right square bracket ']'
    RBracket,

    // preprocessor

    // #!(
    PPBegin,
    // )!
    PPEnd,

    // literal [0-9]+i8  (signed)
    Int8L,
    // literal [0-9]+i16 (signed)
    Int16L,
    // literal [0-9]+i32 (signed) or simply [0-9]+ (default integer type)
    Int32L,
    // literal [0-9]+i64 (signed)
    Int64L,

    // literal [0-9]+u8  (unsigned)
    Uint8L,
    // literal [0-9]+u16 (unsigned)
    Uint16L,
    // literal [0-9]+u32 (unsigned)
    Uint32L,
    // literal [0-9]+u64 (unsigned)
    Uint64L,

    // literal [0-9]*.[0-9]+f32
    Float32L,
    // literal [0-9]*.[0-9]*f64 or simply [0-9]*.[0-9]+ (must have a dot), default float type
    Float64L,

    // string literal (UTF-8) in "" or @"()" (raw string)
    String,
    // character literal (UTF-8) in ''
    Char
};

enum class Keyword
{
    kw_using,
    kw_nspace,
    kw_fn,
    kw_i8,
    kw_i16,
    kw_i32,
    kw_i64,
    kw_u8,
    kw_u16,
    kw_u32,
    kw_u64,
    kw_f32,
    kw_f64,
    kw_ch16,
    kw_str16,
    kw_bool,
    kw_ret,
    kw_mut,
    kw_true,
    kw_false,
    kw_break,
    kw_continue,
    kw_if,
    kw_else,
    kw_while,
    kw_for,
    kw_do,
    kw_let,
    kw_null,
    kw_class,
    kw_pub,
    kw_prv,
    kw_prt,
    kw_vrt,
    kw_base,
    kw_new,
    kw_asm,
    kw_rng,
    kw_in,
    kw_as,
    Last
};

extern const wchar_t* KeywordStr[];

class Token
{
public:
    Token() = default;
    Token(const String& d, TokenType t, Keyword kw = Keyword::Last)
    {
        if (kw != Keyword::Last)
        {
            type = TokenType::Keyword;
        }
        else
        {
            type = t;
        }

        kw_type = kw;
        data = d;
    }

    String data{};
    TokenType type{};
    Keyword kw_type{};
};

struct OperPrec
{
    OperPrec(int p)
    {
        prec = p;
    }

    bool operator<(const OperPrec& p)
    {
        return prec < p.prec;
    }

    bool operator>(const OperPrec& p)
    {
        return prec > p.prec;
    }

    bool operator<=(const OperPrec& p)
    {
        return prec <= p.prec;
    }

    bool operator>=(const OperPrec& p)
    {
        return prec >= p.prec;
    }

    bool operator==(const OperPrec& p)
    {
        return prec == p.prec;
    }

    bool operator!=(const OperPrec& p)
    {
        return prec != p.prec;
    }

    // precedence of the operation
    int prec = 0;
};

OperPrec GetPrecedence(TokenType oper, bool unary);
bool IsNumber(TokenType type);
bool IsNumber(Keyword kw);
bool IsBinaryOp(TokenType type);
Keyword TTypeToKeyword(TokenType t);
size_t GetTypeSize(Keyword kw);
// returns EoF if invalid token type has been passed
TokenType NegateLOp(TokenType op);

