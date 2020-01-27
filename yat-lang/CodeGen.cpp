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
#include "CodeGen.h"
#include <algorithm>

inline Register CodeGen::TryAllocRegister(bool fp, size_t bytes)
{
    if (!fp)
    {
        if (RegisterState.RegB)
        {
            RegisterState.RegB = false;
            switch (bytes)
            {
            case 1: return Register::bl;
            case 2: return Register::bx;
            case 4: return Register::ebx;
            case 8: return Register::rbx;

            default: throw Error(L"Wrong type size");
            }
        }

        for (int i = 0; i < 8; ++i)
        {
            if (RegisterState.RegRx[i])
            {
                RegisterState.RegRx[i] = false;

                switch (bytes)
                {
                case 1: return (Register)((int)Register::r8b + i);
                case 2: return (Register)((int)Register::r8w + i);
                case 4: return (Register)((int)Register::r8d + i);
                case 8: return (Register)((int)Register::r8 + i);

                default: throw Error(L"Wrong type size");
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < 16; ++i)
        {
            if (RegisterState.RegXmm[i])
            {
                RegisterState.RegXmm[i] = false;
                return (Register)((int)Register::xmm0 + i);
            }
        }
    }

    return Register::Last;
}

inline void CodeGen::FreeRegister(Register r)
{
    switch (r)
    {
    case Register::bl:
    case Register::bx:
    case Register::ebx:
    case Register::rbx:
        RegisterState.RegB = true;
        break;
    // case Register::dl:
    // case Register::dx:
    // case Register::edx:
    // case Register::rdx:
    //     RegisterState.RegD = true;
    //     break;
    case Register::r8:
    case Register::r9:
    case Register::r10:
    case Register::r11:
    case Register::r12:
    case Register::r13:
    case Register::r14:
    case Register::r15:
    case Register::r8b:
    case Register::r9b:
    case Register::r10b:
    case Register::r11b:
    case Register::r12b:
    case Register::r13b:
    case Register::r14b:
    case Register::r15b:
    case Register::r8w:
    case Register::r9w:
    case Register::r10w:
    case Register::r11w:
    case Register::r12w:
    case Register::r13w:
    case Register::r14w:
    case Register::r15w:
    case Register::r8d:
    case Register::r9d:
    case Register::r10d:
    case Register::r11d:
    case Register::r12d:
    case Register::r13d:
    case Register::r14d:
    case Register::r15d:
        if ((int)r < (int)Register::r8d)
        {
            RegisterState.RegRx[((int)r) - ((int)Register::r8)] = true;
        }
        else if ((int)r < (int)Register::r8w)
        {
            RegisterState.RegRx[((int)r) - ((int)Register::r8d)] = true;
        }
        else if ((int)r < (int)Register::r8b)
        {
            RegisterState.RegRx[((int)r) - ((int)Register::r8w)] = true;
        }
        else
        {
            RegisterState.RegRx[((int)r) - ((int)Register::r8b)] = true;
        }
        break;
    case Register::xmm0:
    case Register::xmm1:
    case Register::xmm2:
    case Register::xmm3:
    case Register::xmm4:
    case Register::xmm5:
    case Register::xmm6:
    case Register::xmm7:
    case Register::xmm8:
    case Register::xmm9:
    case Register::xmm10:
    case Register::xmm11:
    case Register::xmm12:
    case Register::xmm13:
    case Register::xmm14:
    case Register::xmm15:
        RegisterState.RegXmm[((int)r) - ((int)Register::xmm0)] = true;
        break;
    }
}

String CodeGen::GenLabel()
{
    static size_t count = 0;
    return L".L" + std::to_wstring(count++);
}

CodeGen::LocalVar CodeGen::GetLocal(Var* v)
{
    for (StackFrame& f : locals)
    {
        for (LocalVar& loc : f.vars)
        {
            if (loc.data == v || loc.data->name == v->name)
            {
                return loc;
            }
        }
    }

    return LocalVar();
}

void CodeGen::VisitNSpace(Namespace* ns)
{
    cur_ns = ns;
    VisitBlock(ns->block, true, init);
}

void CodeGen::VisitBlock(StatementBlock* b, bool glob, std::vector<AsInstr>& res)
{
    for (ASTNode* node : b->children)
    {
        const auto vres = VisitNode(node, glob, res);
        if (vres.type == VisitRes::reg)
        {
            FreeRegister(vres.rData);
        }
    }
}

inline void CodeGen::SolveCondition(ASTNode* cond, std::vector<AsInstr>& res, const AsInstr& jumpt, const AsInstr& jumpf)
{
    BinOp* op = (BinOp*)cond;

    if (op->oper.type == TokenType::OperLAnd)
    {
        AsInstr next_l(GenLabel(), true);
        SolveCondition(op->r, res, next_l, jumpf);
        res.push_back(next_l);
        SolveCondition(op->l, res, jumpt, jumpf);
        return;
    }

    if (op->oper.type == TokenType::OperLOr)
    {
        AsInstr next_l(GenLabel(), true);
        SolveCondition(op->r, res, jumpt, next_l);
        res.push_back(next_l);
        SolveCondition(op->l, res, jumpt, jumpf);
        return;
    }

    auto vres = VisitNode(op, false, res);

    AsInstr::InstrSuffix jcond{};
    switch (vres.bData)
    {
    case TokenType::OperLess:
        jcond = AsInstr::InstrSuffix::c_l;
        break;
    case TokenType::OperGreater:
        jcond = AsInstr::InstrSuffix::c_g;
        break;

    case TokenType::OperEqual:
        jcond = AsInstr::InstrSuffix::c_e;
        break;
    case TokenType::OperNEqual:
        jcond = AsInstr::InstrSuffix::c_ne;
        break;

    case TokenType::OperLEqual:
        jcond = AsInstr::InstrSuffix::c_le;
        break;
    case TokenType::OperGEqual:
        jcond = AsInstr::InstrSuffix::c_ge;
        break;
    default:
        throw Error(L"invalid condition");
    }

    AsInstr cj;
    cj.suf = jcond;
    cj.instr = AsInstr::Instr::as_j;
    cj.oper1 = AsInstr::Operands::Label;
    cj.l1 = jumpf.l1;

    AsInstr jmp;
    jmp.instr = AsInstr::Instr::as_jmp;
    jmp.oper1 = AsInstr::Operands::Label;
    jmp.l1 = jumpt.l1;

    res.push_back(cj);
    res.push_back(jmp);
}

inline AsInstr CodeGen::MakeMov(const VisitRes& from, const VisitRes& to, const size_t op_size)
{
    AsInstr mov_in;
    mov_in.instr = AsInstr::Instr::as_mov;
    mov_in.SetSizeSuffix(op_size);

    switch (from.type)
    {
    case VisitRes::glob:
    {
        if (from.gData->var_type == Keyword::kw_str16)
        {
            // change instruction to 'lea' cause we want pointer to be copied
            mov_in.instr = AsInstr::Instr::as_lea;
        }
        mov_in.oper1 = AsInstr::Operands::Label;
        mov_in.l1 = from.gData->name;
        break;
    }
    case VisitRes::loc:
    {
        mov_in.oper1 = AsInstr::Operands::Stack;
        mov_in.mem1 = from.lData.offset;
        break;
    }
    case VisitRes::reg:
    {
        FreeRegister(from.rData);
        mov_in.oper1 = AsInstr::Operands::Reg;
        mov_in.reg1 = from.rData;
        break;
    }
    case VisitRes::func:
    {
        // change instruction to 'lea' cause we want pointer to be copied
        mov_in.instr = AsInstr::Instr::as_lea;
        mov_in.oper1 = AsInstr::Operands::Label;
        mov_in.l1 = from.fData;
        break;
    }
    case VisitRes::cnst:
    {
        mov_in.oper1 = AsInstr::Operands::Const;
        mov_in.l1 = from.cData->data.data;
        break;
    }
    }

    switch (to.type)
    {
    case VisitRes::glob:
    {
        mov_in.oper2 = AsInstr::Operands::Label;
        mov_in.l2 = to.gData->name;
        break;
    }
    case VisitRes::loc:
    {
        mov_in.oper2 = AsInstr::Operands::Stack;
        mov_in.mem2 = to.lData.offset;
        break;
    }
    case VisitRes::reg:
    {
        mov_in.oper2 = AsInstr::Operands::Reg;
        mov_in.reg2 = to.rData;
        break;
    }
    default:
        throw Error(L"Compiler Error: invalid destination in mov instruction");
    }

    return mov_in;
}

CodeGen::VisitRes CodeGen::VisitNode(ASTNode* node, bool glob, std::vector<AsInstr>& res)
{
    switch (node->type)
    {
    case NodeType::Cvt:
    {
        Convert* cvt = (Convert*)node;
        VisitRes vr = VisitNode(cvt->value, glob, res);
        Register temp = TryAllocRegister(false, GetTypeSize(cvt->value->GetTypeKW()));

        AsInstr mov_in = MakeMov(vr, temp, GetTypeSize(cvt->value->GetTypeKW()));

        AsInstr inst;
        inst.instr = AsInstr::Instr::as_mov;
        inst.oper1 = AsInstr::Operands::Reg;
        inst.reg1 = temp;

        if (GetTypeSize(cvt->value->GetTypeKW()) < GetTypeSize(cvt->GetTypeKW())
            && IsSigned(cvt->GetTypeKW()))
        {
            switch (cvt->value->GetTypeKW())
            {
            case Keyword::kw_i8:
            case Keyword::kw_i16:
            case Keyword::kw_i32:
            case Keyword::kw_i64:
                inst.suf = AsInstr::InstrSuffix::x_s;
                break;/*
            case Keyword::kw_u8:
            case Keyword::kw_u16:
            case Keyword::kw_u32:
            case Keyword::kw_u64:
                inst.suf = AsInstr::InstrSuffix::x_z;
                break;*/
            }

            inst.SetSizeSuffix(GetTypeSize(cvt->value->GetTypeKW()));
            inst.SetSizeSuffix(GetTypeSize(cvt->GetTypeKW()));
        }
        else
        {

            inst.SetSizeSuffix(GetTypeSize(cvt->GetTypeKW()));
            inst.reg1 = CvtReg(inst.reg1, GetTypeSize(cvt->GetTypeKW()));
        }

        inst.oper2 = AsInstr::Operands::Reg;
        inst.reg2 = TryAllocRegister(false, GetTypeSize(cvt->GetTypeKW()));

        FreeRegister(temp);

        res.push_back(mov_in);
        res.push_back(inst);

        return VisitRes(inst.reg2);
    }
    case NodeType::WhileLoop:
    {
        WhileLoop* lp = (WhileLoop*)node;
        AsInstr start_l(GenLabel(), true), end_l(GenLabel(), true), endc_l(GenLabel(), true);
        start_l.isLabel = end_l.isLabel = true;

        res.push_back(AsInstr(L"#while start\n", false));
        res.push_back(start_l);

        // m_cond = true;
        // jcondf = end_l;
        // jcondt = endc_l;
        SolveCondition(lp->condition, res, endc_l, end_l);
        // m_cond = false;

        res.push_back(endc_l);

        VisitBlock(lp->body, glob, res);
        AsInstr jmp_start;
        jmp_start.suf = AsInstr::InstrSuffix::Last;
        jmp_start.instr = AsInstr::Instr::as_jmp;
        jmp_start.l1 = start_l.l1;
        jmp_start.oper1 = AsInstr::Operands::Label;
        res.push_back(jmp_start);

        res.push_back(AsInstr(L"#while end\n", false));
        res.push_back(end_l);

        break;
    }
    case NodeType::IfSt:
    {
        IfStatement* is = (IfStatement*)node;

        AsInstr end_then(GenLabel(), true), s_then(GenLabel(), true);
        end_then.isLabel = true;

        // m_cond = true;
        // jcondf = end_then;
        // jcondt = s_then;
        SolveCondition(is->condition, res, s_then, end_then);
        // m_cond = false;

        res.push_back(s_then);

        VisitBlock(is->then_b, false, res);

        if (is->else_b)
        {
            String end_else = GenLabel();

            AsInstr cj;
            cj.suf = AsInstr::InstrSuffix::Last;
            cj.instr = AsInstr::Instr::as_jmp;
            cj.oper1 = AsInstr::Operands::Label;
            cj.l1 = end_else;

            res.push_back(cj);

            res.push_back(end_then);
            res.push_back(AsInstr(L"#if-then end\n"));

            VisitBlock(is->else_b, false, res);

            res.push_back(AsInstr(end_else, true));
            res.push_back(AsInstr(L"#if-else end\n"));
        }
        else
        {
            res.push_back(end_then);
            res.push_back(AsInstr(L"#if-then end\n"));
        }

        return VisitRes();
    }
    case NodeType::Call:
    {
        FnCall* v = (FnCall*)node;

        int64_t param_bytes = 0;

        // push all parameters in reversed order
        for (auto iter = v->params.crbegin(); iter != v->params.crend(); ++iter)
        {
            ASTNode* param = *iter;
            VisitRes vr = VisitNode(param, false, res);

            param_bytes += GetTypeSize(param->GetTypeKW());

            AsInstr mov_in;
            mov_in.instr = AsInstr::Instr::as_mov;
            mov_in.SetSizeSuffix(GetTypeSize(param->GetTypeKW()));

            mov_in.oper2 = AsInstr::Operands::Reg;
            mov_in.reg2 = TryAllocRegister(false, GetTypeSize(param->GetTypeKW()));

            AsInstr push_in;
            push_in.SetSizeSuffix(GetTypeSize(param->GetTypeKW()));
            push_in.instr = AsInstr::Instr::as_mov;
            push_in.oper1 = AsInstr::Operands::Reg;
            push_in.reg1 = mov_in.reg2;

            push_in.mem2 = -param_bytes;
            push_in.stackReg = Register::rsp;
            push_in.oper2 = AsInstr::Operands::Stack;

            switch (vr.type)
            {
            case VisitRes::glob:
            {
                if (vr.gData->var_type == Keyword::kw_str16)
                {
                    mov_in.instr = AsInstr::Instr::as_lea;
                }
                mov_in.oper1 = AsInstr::Operands::Label;
                mov_in.l1 = vr.gData->name;
                break;
            }
            case VisitRes::loc:
            {
                mov_in.oper1 = AsInstr::Operands::Stack;
                mov_in.mem1 = vr.lData.offset;
                break;
            }
            case VisitRes::reg:
            {
                FreeRegister(vr.rData);
                mov_in.oper1 = AsInstr::Operands::Reg;
                mov_in.reg1 = vr.rData;
                break;
            }
            case VisitRes::func:
            {
                // change instruction to 'lea' cause we want pointer to be copied
                mov_in.instr = AsInstr::Instr::as_lea;
                mov_in.oper1 = AsInstr::Operands::Label;
                mov_in.l1 = vr.fData;
                break;
            }
            case VisitRes::cnst:
            {
                mov_in.oper1 = AsInstr::Operands::Const;
                mov_in.l1 = vr.cData->data.data;
                break;
            }
            }

            res.push_back(mov_in);
            res.push_back(push_in);
            FreeRegister(mov_in.reg2);
        }

        String instr = v->FnName.data;

        res.push_back(L"subq      $" + std::to_wstring(param_bytes) + L", %rsp\n");

        AsInstr call_in(L"call      *(" + instr + L")\n");

        res.push_back(call_in);

        res.push_back(L"addq      $" + std::to_wstring(param_bytes) + L", %rsp\n");

        if (v->func->ret_type != Keyword::kw_null)
        {
            Register ret_r = Register::Last;
            switch (GetTypeSize(v->func->ret_type))
            {
            case 1:
                ret_r = Register::ah;
                break;
            case 2:
                ret_r = Register::ax;
                break;
            case 4:
                ret_r = Register::eax;
                break;
            case 8:
                ret_r = Register::rax;
                break;
            }
            return VisitRes(ret_r);
        }

        return VisitRes();
    }
    case NodeType::String:
    {
        String label = GenLabel();
        Var* v = new Var(label, Keyword::kw_str16);

        strings.push_back(std::make_pair(label, ((StrLeaf*)node)->data.data));

        return VisitRes(v);
    }
    case NodeType::ConstLeaf:
    {
        return VisitRes((ConstLeaf*)node);
    }
    case NodeType::Var:
    {
        Var* v = (Var*)node;
        if (glob)
        {
            globals.push_back(v);
        }
        else
        {
            locals[locals.size() - 1].AddVar(v);
        }

        if (v->initial)
        {
            BinOp* init_op = new BinOp();
            init_op->l = new VarLeaf(v);
            init_op->r = v->initial;
            init_op->oper = Token(L"=", TokenType::Assign, 0, 0, 0);

            VisitRes ivr = VisitNode(init_op, glob, res);
            if (ivr.type == VisitRes::reg)
            {
                FreeRegister(ivr.rData);
            }
        }

        return VisitRes();
    }
    case NodeType::VarLeaf:
    {
        VarLeaf* v = (VarLeaf*)node;
        LocalVar lv = GetLocal(v->data);

        if (lv.data)
        {
            return VisitRes(lv);
        }

        return VisitRes(v->data);
    }
    case NodeType::UnOper:
    {
        UnOp* op = (UnOp*)node;
        switch (op->oper.type)
        {
        case TokenType::Keyword:
        {
            if (op->oper.kw_type == Keyword::kw_asm)
            {
                ConstLeaf* asm_text = (ConstLeaf*)op->operand;
                res.push_back(AsInstr(asm_text->data.data));
                return VisitRes();
            }

            if (op->oper.kw_type == Keyword::kw_ret)
            {
                VisitRes oper_vis = VisitNode(op->operand, glob, res);

                Register reg{};

                switch (GetTypeSize(op->GetTypeKW()))
                {
                case 1:
                    reg = Register::ah;
                    break;
                case 2:
                    reg = Register::ax;
                    break;
                case 4:
                    reg = Register::eax;
                    break;
                case 8:
                    reg = Register::rax;
                    break;
                }
                AsInstr mov_in = MakeMov(oper_vis, reg, GetTypeSize(op->GetTypeKW()));

                res.push_back(mov_in);

                // stack frame leave:
                res.push_back(AsInstr(L"leave\n"));

                AsInstr ret_in;
                ret_in.instr = AsInstr::Instr::as_ret;
                res.push_back(ret_in);
            }
            break;
        }
        case TokenType::OperMin: // unary minus
            // TODO: prefix and postfix increments
        case TokenType::OperInc: // increment
        case TokenType::OperDec: // decrement
        {
            uint64_t size = GetTypeSize(op->GetTypeKW());
            Register temp = TryAllocRegister(false, size);


        }
        }
        break;
    }
    case NodeType::BinOper:
    {
        BinOp* op = (BinOp*)node;

        Token noper(L"", TokenType::EoF, 0, 0, 0);
        switch (op->oper.type)
        {
        case TokenType::AssignPlus:
            noper.type = TokenType::OperPlus;
            break;

        case TokenType::AssignMin:
            noper.type = TokenType::OperMin;
            break;

        case TokenType::AssignMul:
            noper.type = TokenType::OperMul;
            break;

        case TokenType::AssignPow:
            noper.type = TokenType::OperPow;
            break;

        case TokenType::AssignDiv:
            noper.type = TokenType::OperDiv;
            break;

        case TokenType::AssignPCent:
            noper.type = TokenType::OperPCent;
            break;

        case TokenType::AssignLShift:
            noper.type = TokenType::OperLShift;
            break;

        case TokenType::AssignRShift:
            noper.type = TokenType::OperRShift;
            break;

        case TokenType::AssignBWAnd:
            noper.type = TokenType::OperBWAnd;
            break;

        case TokenType::AssignBWOr:
            noper.type = TokenType::OperBWOr;
            break;

        case TokenType::AssignXor:
            noper.type = TokenType::OperXor;
            break;
        }

        if (noper.type != TokenType::EoF)
        {
            op->oper = noper;
            BinOp* assign = new BinOp();
            assign->oper = Token(L"=", TokenType::Assign, 0, 0, 0);
            assign->r = op;
            assign->l = op->r;

            return VisitNode(assign, glob, res);
        }

        VisitRes left_vis = VisitNode(op->l, glob, res);
        VisitRes right_vis = VisitNode(op->r, glob, res);

        size_t op_size  = GetTypeSize(op->GetTypeKW());

        /*
        left <op> right

        1.Get temporary register (or stack) <temp>

        2.Write Assembly:
            mov right, <temp>
            <op> left, <temp>

        3.If left or right is register:
            Free Register left or right

        4.Return <temp>
        */

        Register temp;
        AsInstr mov_in;
        if (right_vis.type != VisitRes::reg || op->oper.type == TokenType::OperDiv ||
            (op->oper.type == TokenType::OperMul && !IsSigned(op->GetTypeKW())))
        {
            temp = TryAllocRegister(false, op_size);

            mov_in = MakeMov(right_vis, temp, op_size);
        }
        else
        {
            temp = right_vis.rData;
        }

        AsInstr inst;
        if (op->GetTypeKW() != Keyword::kw_bool)
        {
            inst.instr = TTypeToInstr(op->oper.type, IsSigned(op->GetTypeKW()));
        }
        inst.SetSizeSuffix(op_size);

        switch (left_vis.type)
        {
        case VisitRes::glob:
        {
            inst.oper1 = AsInstr::Operands::Label;
            inst.l1 = left_vis.gData->name;
            break;
        }
        case VisitRes::loc:
        {
            inst.oper1 = AsInstr::Operands::Stack;
            inst.mem1 = left_vis.lData.offset;
            break;
        }
        case VisitRes::reg:
        {
            FreeRegister(left_vis.rData);
            inst.oper1 = AsInstr::Operands::Reg;
            inst.reg1 = left_vis.rData;
            break;
        }
        case VisitRes::cnst:
        {
            inst.oper1 = AsInstr::Operands::Const;
            inst.l1 = left_vis.cData->data.data;
            break;
        }
        }
        inst.oper2 = AsInstr::Operands::Reg;
        inst.reg2 = temp;

        TokenType bool_t = NegateLOp(op->oper.type);
        if (bool_t != TokenType::EoF)
        {
            // compare values
            inst.instr = AsInstr::Instr::as_cmp;
            if (right_vis.type != VisitRes::reg)
                res.push_back(mov_in);
            res.push_back(inst);

            return VisitRes(bool_t);
        }

        if (op->oper.type == TokenType::Assign)
        {
            // this code gives incorrect assembly for assignemet operator, e.g.
            // assembly for 'program.var = 2':
            //     movq $2, %rbx
            //     movq program.var, %rbx
            // to fix this we have to swap operands of the second instruction

            inst.SwapOperands();
            if (right_vis.type != VisitRes::reg)
                res.push_back(mov_in);
            res.push_back(inst);
        }
        // Special case: divide and multiply (unsigned)
        // assume the operands are already converted to 64-bit
        else if (op->oper.type == TokenType::OperDiv ||
            (op->oper.type == TokenType::OperMul && !IsSigned(op->GetTypeKW())))
        {
            mov_in.reg2 = Register::rax;
            res.push_back(mov_in);

            res.push_back(MakeMov(left_vis, temp, op_size));
            if (IsSigned(op->GetTypeKW()))
            {
                res.push_back(AsInstr(L"cqto\n")); // signed extend of dividend
            }
            else
            {
                // unsigned extend of dividend
                AsInstr uext;
                uext.instr = AsInstr::Instr::as_xor;
                uext.SetSizeSuffix(8);
                uext.oper1 = uext.oper2 = AsInstr::Operands::Reg;
                uext.reg1 = uext.reg2 = Register::rdx;
                res.push_back(uext);
            }

            inst.oper1 = AsInstr::Operands::Reg;
            inst.reg1 = temp;
            inst.oper2 = AsInstr::Operands::Last;
            res.push_back(inst);
            res.push_back(MakeMov(mov_in.reg2, temp, op_size));
        }
        else
        {
            if (right_vis.type != VisitRes::reg)
                res.push_back(mov_in);
            res.push_back(inst);
        }
        return VisitRes(temp);
    }
    case NodeType::Func:
    {
        Lambda* v = (Lambda*)node;

        size_t param_bytes = 0;
        for (Var* p : v->params)
        {
            param_bytes += GetTypeSize(p->GetTypeKW());
        }

        locals.push_back(StackFrame(param_bytes)); // push param list
        for (Var* p : v->params)
        {
            p->is_param = true;
            locals[locals.size() - 1].AddVar(p);
        }

        String label = GenLabel();
        func.push_back(std::make_pair(label, std::vector<AsInstr>()));

        /*
        Stack frame enter instructions:
            pushq   %rbp
            movq    %rsp, %rbp
            subq    $<size of all locals of the function (in bytes)>, %rsp
        */
        AsInstr enter_in[3];

        enter_in[0].instr = AsInstr::Instr::as_push;
        enter_in[0].SetSizeSuffix(8);
        enter_in[0].oper1 = AsInstr::Operands::Reg;
        enter_in[0].reg1 = Register::rbp;

        enter_in[1] = MakeMov(Register::rsp, Register::rbp, 8);

        enter_in[2].instr = AsInstr::Instr::as_sub;
        enter_in[2].SetSizeSuffix(8);
        enter_in[2].oper1 = AsInstr::Operands::Const;
        // we add 32, because program segfaults if allocated stack is less
        enter_in[2].l1 = std::to_wstring(v->def->bytes + 32);
        enter_in[2].oper2 = AsInstr::Operands::Reg;
        enter_in[2].reg2 = Register::rsp;

        for (int i = 0; i < 3; ++i)
        {
            func[func.size() - 1].second.push_back(enter_in[i]);
        }

        VisitBlock(v->def, false, func[func.size() - 1].second);

        if (v->ret_type == Keyword::kw_null)
        {
            func[func.size() - 1].second.push_back(AsInstr(L"leave\n\tret\n"));
        }

        locals.pop_back(); // pop param list
        return VisitRes(label);
    }
    }

    return VisitRes();
}

CodeGen::CodeGen(AST& ast)
{
    m_ast = &ast;
}

void CodeGen::WriteCode(const String& path)
{
    stream = std::wofstream(path);

    for (int i = 0; i < 16; ++i)
    {
        RegisterState.RegXmm[i] = true;
    }

    for (int i = 0; i < 8; ++i)
    {
        RegisterState.RegRx[i] = true;
    }

    for (Namespace* ns : m_ast->prog)
    {
        VisitNSpace(ns);
    }

    stream << L".text\n\t.globl main\n";
    stream << L"main:\n";

    for (AsInstr& instr : init)
    {
        if (instr.isLabel)
        {
            stream << instr.GenText() << L":\n";
        }
        else
        {
            stream << L"\t" << instr.GenText();
        }
    }
    stream
        << L"\tcall      *(program.main)\n"
        << L"\tret\n\n";

    stream << L".data\n";

    for (auto& pair : strings)
    {
        stream << pair.first << L":\n\t.hword ";
        for (wchar_t c : pair.second)
        {
            stream << (int)c << L", ";
        }

        stream << L"0\n";
    }

    for (Var* g : globals)
    {
        stream << g->name << L":\n\t.zero " << GetTypeSize(g->GetTypeKW()) << L"\n";
    }

    stream << L".text\n";

    for (auto& fn : func)
    {
        stream << fn.first << L":\n";
        for (auto& instr : fn.second)
        {
            if (instr.isLabel)
            {
                stream << instr.GenText() << L":\n";
            }
            else
            {
                stream << L"\t" << instr.GenText();
            }
        }
    }

    stream << L"\n";
}

CodeGen::StackFrame::StackFrame(int64_t off)
{
    stack_offset = off;
}

void CodeGen::StackFrame::AddVar(Var* v)
{
    LocalVar var;
    var.data = v;
    stack_offset -= GetTypeSize(v->var_type);
    var.offset = stack_offset;
    if (v->is_param)
    {
        var.offset += 16;
    }
    vars.push_back(var);
}

CodeGen::VisitRes::VisitRes()
{
    type = none;
}

CodeGen::VisitRes::VisitRes(ConstLeaf* c)
{
    cData = c;
    type = cnst;
}

CodeGen::VisitRes::VisitRes(Register r)
{
    rData = r;
    type = reg;
}

CodeGen::VisitRes::VisitRes(Var* g)
{
    gData = g;
    type = glob;
}

CodeGen::VisitRes::VisitRes(const LocalVar& l)
{
    lData = l;
    type = loc;
}

CodeGen::VisitRes::VisitRes(const String& f)
{
    fData = f;
    type = func;
}

CodeGen::VisitRes::VisitRes(const TokenType b)
{
    bData = b;
    type = cond;
}

