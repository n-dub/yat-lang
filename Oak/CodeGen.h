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
#include "AST.h"
#include "Utils.h"

#pragma warning(disable : 26812)

/*
  x86_64 register (_ is a, b, c or d)
  ================ r_x (64 bits)
  ________======== e_x (32 bits)
  ____________====  _x (16 bits)
  ____________==__  _h (8  bits)
  ______________==  _l (8  bits)
*/
enum class Register
{
    al,   // register a (x8)
    bl,   // register b (x8)
    cl,   // register c (x8)
    dl,   // register d (x8)

    ah,   // register a (x8)
    bh,   // register b (x8)
    ch,   // register c (x8)
    dh,   // register d (x8)

    ax,    // register a (x16)
    bx,    // register b (x16)
    cx,    // register c (x16)
    dx,    // register d (x16)

    eax,   // register a (x32)
    ebx,   // register b (x32)
    ecx,   // register c (x32)
    edx,   // register d (x32)

    rax,   // register a (x64)
    rbx,   // register b (x64)
    rcx,   // register c (x64)
    rdx,   // register d (x64)

    // aren't used to store variables
    rbp, // base pointer
    rsp, // stack pointer
    rsi, // source index
    rdi, // destination index

    r8,    // register 8  (x64)
    r9,    // register 9  (x64)
    r10,   // register 10 (x64)
    r11,   // register 11 (x64)
    r12,   // register 12 (x64)
    r13,   // register 13 (x64)
    r14,   // register 14 (x64)
    r15,   // register 15 (x64)

    r8d,    // register 8  (x32)
    r9d,    // register 9  (x32)
    r10d,   // register 10 (x32)
    r11d,   // register 11 (x32)
    r12d,   // register 12 (x32)
    r13d,   // register 13 (x32)
    r14d,   // register 14 (x32)
    r15d,   // register 15 (x32)

    r8w,    // register 8  (x16)
    r9w,    // register 9  (x16)
    r10w,   // register 10 (x16)
    r11w,   // register 11 (x16)
    r12w,   // register 12 (x16)
    r13w,   // register 13 (x16)
    r14w,   // register 14 (x16)
    r15w,   // register 15 (x16)

    r8b,    // register 8  (x8)
    r9b,    // register 9  (x8)
    r10b,   // register 10 (x8)
    r11b,   // register 11 (x8)
    r12b,   // register 12 (x8)
    r13b,   // register 13 (x8)
    r14b,   // register 14 (x8)
    r15b,   // register 15 (x8)

    xmm0,  // register 0  (x128)
    xmm1,  // register 1  (x128)
    xmm2,  // register 2  (x128)
    xmm3,  // register 3  (x128)
    xmm4,  // register 4  (x128)
    xmm5,  // register 5  (x128)
    xmm6,  // register 6  (x128)
    xmm7,  // register 7  (x128)
    xmm8,  // register 8  (x128)
    xmm9,  // register 9  (x128)
    xmm10, // register 10 (x128)
    xmm11, // register 11 (x128)
    xmm12, // register 12 (x128)
    xmm13, // register 13 (x128)
    xmm14, // register 14 (x128)
    xmm15, // register 15 (x128)

    Last
};

extern const String RegisterStr[];

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
    };

    Operands oper1 = Operands::Last;
    Operands oper2 = Operands::Last;

    Register reg1{}, reg2{}, stackReg = Register::rbp;
    int64_t mem1{}, mem2{};
    String l1, l2;
    bool isLabel = false;

    AsInstr() = default;
    AsInstr(const String& inl);

    String GenText();
    void SetSizeSuffix(size_t bytes);
    void SwapOperands();
};

class CodeGen
{
    struct LocalVar
    {
        int64_t offset = 0;
        Var* data = nullptr;
    };

    struct StackFrame
    {
        std::vector<LocalVar> vars;
        int64_t stack_offset = 0;

        StackFrame() = default;
        StackFrame(int64_t off);

        inline void AddVar(Var* v);
    };

    std::vector<StackFrame> locals;

    // initialization instructions
    // there we have to pass initial values to the
    // variables and call fn program.main
    std::vector<AsInstr> init;
    // definitions of all global variables
    std::vector<Var*> globals;
    // all string literals
    // PAIRS:
    // Label : literal
    std::vector<std::pair<String, String>> strings;
    // definitions of all functions (lambdas)
    // PAIRS:
    // Label : vector of instructions
    std::vector<std::pair<String, std::vector<AsInstr>>> func;

    AST* m_ast = nullptr;
    std::wofstream stream;
    Namespace* cur_ns = nullptr;

    // node visiting result
    struct VisitRes
    {
        enum
        {
            // integer constant
            cnst,
            // register
            reg,
            // global
            glob,
            // local
            loc,
            // function (lambda)
            func,
            // condition (boolean)
            cond,
            // no return
            none
        } type = none;

        VisitRes();
        // constant
        VisitRes(ConstLeaf* c);
        // register
        VisitRes(Register r);
        // global
        VisitRes(Var* g);
        // local
        VisitRes(const LocalVar& l);
        // 
        VisitRes(const String& f);
        // boolean
        VisitRes(const TokenType b);

        Register rData{};
        Var* gData{};
        ConstLeaf* cData{};
        LocalVar lData{};
        String fData{};
        TokenType bData{};
    };

    struct _regs
    {
        // bool RegA = true; // rax register is reserved for fn return value
        // bool RegC = true; // rcx register is reserved for 'this' pointer

        bool RegB = true; // TRUE if the register is free
        bool RegD = true; // TRUE if the register is free

        bool RegXmm[16]{};  // TRUE if the register is free

        bool RegRx[8]{};    // TRUE if the register is free (r8 - r15)
    } RegisterState;

    inline Register TryAllocRegister(bool fp, size_t bytes);
    inline void FreeRegister(Register r);

    inline String GenLabel();
    inline LocalVar GetLocal(Var* v);

    inline void SolveCondition(const VisitRes& cond, std::vector<AsInstr>& res, const AsInstr& jump);

    void VisitNSpace(Namespace* ns);
    void VisitBlock(StatementBlock* b, bool glob, std::vector<AsInstr>& res);
    VisitRes VisitNode(ASTNode* node, bool glob, std::vector<AsInstr>& res);

public:
    CodeGen(AST& ast);
    void WriteCode(const String& path);
};

