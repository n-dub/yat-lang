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
#include "Utils.h"
#include "Register.h"
#include "Tokens.h"
// assembly instruction
struct AsInstr
{
    enum class InstrSuffix
    {
        s_b,   // byte
        s_s,   // single (32 - bit float)
        s_w,   // word
        s_l,   // long (doubleword)
        s_q,   // quadword

        c_g,   // greater
        c_l,   // less
        c_e,   // equal
        c_ge,  // greater or equal
        c_le,  // less or equal
        c_ng,  // not greater
        c_nl,  // not less
        c_ne,  // not equal
        c_nge, // not greater or equal
        c_nle, // not less or equal

        x_z,   // zero extend
        x_s,   // signed extend

        Last
    } suf = InstrSuffix::Last, suf0 = InstrSuffix::Last, suf1 = InstrSuffix::Last;

    static const String SuffixStr[];

    enum class Instr
    {
        as_nop,  // no operation
        as_mov,  // copy instruction
        as_lea,  // load effective address
        as_add,  // add
        as_sub,  // subtract
        as_mul,  // multiply
        as_imul, // multiply
        as_div,  // divide
        as_idiv, // divide
        as_push, // push to the stack
        as_pop,  // pop from the stack
        as_call, // call a function
        as_cmp,  // compare
        as_dec,  // decrement
        as_inc,  // increment
        as_jmp,  // jump
        as_j,    // conditional jump (use with generated suffixes, e.g. e, ge, l, etc.)
        as_ret,

        as_shl,
        as_shr,
        as_and,
        as_or,
        as_not,
        as_xor,
        as_neg,
        Last
    } instr = Instr::as_nop;

    static const String InstrStr[];

    enum class Operands
    {
        Reg,
        Stack,
        Label,
        Const,
        Last
    } oper1 = Operands::Last, oper2 = Operands::Last;

    Register reg1{}, reg2{}, stackReg = Register::rbp;
    int64_t mem1{}, mem2{};
    String l1, l2;
    bool isLabel = false;

    AsInstr() = default;
    AsInstr(const String& inl, bool lab = false);

    String GenText();
    void SetSizeSuffix(size_t bytes);
    void SwapOperands();
};

AsInstr::Instr TTypeToInstr(TokenType type);

