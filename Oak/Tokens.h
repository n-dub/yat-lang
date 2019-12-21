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

    // literal [0-9]+i8  (signed)
    Int8L,
    // literal [0-9]+i16 (signed)
    Int16L,
    // literal [0-9]+i32 (signed) or simply [0-9]+ (default number type)
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
    kw_string,
    kw_i8,
    kw_i16,
    kw_i32,
    kw_i64,
    kw_u8,
    kw_u16,
    kw_u32,
    kw_u64,
    kw_f16,
    kw_f32,
    kw_f64,
    kw_f128,
    kw_ch16,
    kw_ch32,
    kw_str16,
    kw_str32,
    kw_bool,
    kw_ret,
    kw_mut,
    kw_true,
    kw_false,
    kw_break,
    kw_continue,
    kw_if,
    kw_else,
    kw_switch,
    kw_case,
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
    Last
};

const wchar_t* KeywordStr[]{
    L"using",
    L"nspace",
    L"fn",
    L"string",
    L"i8",
    L"i16",
    L"i32",
    L"i64",
    L"u8",
    L"u16",
    L"u32",
    L"u64",
    L"f16",
    L"f32",
    L"f64",
    L"f128",
    L"ch16",
    L"ch32",
    L"str16",
    L"str32",
    L"bool",
    L"ret",
    L"mut",
    L"true",
    L"false",
    L"break",
    L"continue",
    L"if",
    L"else",
    L"switch",
    L"case",
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
    L"new"
};

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

