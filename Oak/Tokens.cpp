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
    L"f32",
    L"f64",
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

