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
#include "AST.h"
#include "Utils.h"
#include "Register.h"
#include "AsInstr.h"

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

