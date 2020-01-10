#include "AsInstr.h"
#include "Tokens.h"

const String AsInstr::SuffixStr[]{
    L"b", L"s", L"w", L"l", L"q",
    L"g",   L"l",  L"e",
    L"ge",  L"le",
    L"ng",  L"nl", L"ne",
    L"nge", L"nle",
    L"z",   L"s",
    L""
};

const String AsInstr::InstrStr[]{
    L"nop",
    L"mov",
    L"lea",
    L"add",
    L"sub",
    L"mul",
    L"imul",
    L"div",
    L"idiv",
    L"push",
    L"pop",
    L"call",
    L"cmp",
    L"dec",
    L"inc",
    L"jmp",
    L"j",
    L"ret",
    L"shl",
    L"shr",
    L"and",
    L"or",
    L"not",
    L"xor",
    L"neg",
    L""
};

AsInstr::Instr TTypeToInstr(TokenType type)
{
    switch (type)
    {
    case TokenType::Assign:
        return AsInstr::Instr::as_mov;
    case TokenType::OperPlus:
        return AsInstr::Instr::as_add;
    case TokenType::OperMin:
        return AsInstr::Instr::as_sub;
    case TokenType::OperMul:
        return AsInstr::Instr::as_imul;
    case TokenType::OperPow:
        throw; // TODO
    case TokenType::OperDiv:
        return AsInstr::Instr::as_idiv;
    case TokenType::OperPCent:
        throw; // TODO
    case TokenType::OperInc:
        return AsInstr::Instr::as_inc;
    case TokenType::OperDec:
        return AsInstr::Instr::as_dec;
    case TokenType::OperLShift:
        return AsInstr::Instr::as_shl;
    case TokenType::OperRShift:
        return AsInstr::Instr::as_shr;
    case TokenType::OperBWAnd:
        return AsInstr::Instr::as_and;
    case TokenType::OperBWOr:
        return AsInstr::Instr::as_or;
    case TokenType::OperNot:
        return AsInstr::Instr::as_not;
    case TokenType::OperXor:
        return AsInstr::Instr::as_xor;
    default:
        throw;
    }
}

AsInstr::AsInstr(const String& inl)
{
    l1 = inl;
    instr = Instr::Last;
}

String AsInstr::GenText()
{
    if (instr == Instr::Last)
    {
        return l1;
    }

    String res = L"";
    res += InstrStr[(size_t)instr];
    res += SuffixStr[(size_t)suf];
    res += SuffixStr[(size_t)suf0];
    res += SuffixStr[(size_t)suf1];

    for (int i = res.length(); i < 10; ++i)
    {
        res += L" ";
    }

    switch (oper1)
    {
    case AsInstr::Operands::Reg:
        res += L"%";
        res += RegisterStr[(size_t)reg1];
        break;
    case AsInstr::Operands::Stack:
        res += std::to_wstring(mem1);
        res += L"(%";
        res += RegisterStr[(size_t)stackReg];
        res += L")";
        break;
    case AsInstr::Operands::Label:
        res += l1;
        break;
    case AsInstr::Operands::Const:
        res += L"$";
        res += l1;
        break;
    }

    switch (oper2)
    {
    case AsInstr::Operands::Reg:
        res += L", %";
        res += RegisterStr[(size_t)reg2];
        break;
    case AsInstr::Operands::Stack:
        res += L", ";
        res += std::to_wstring(mem2);
        res += L"(%";
        res += RegisterStr[(size_t)stackReg];
        res += L")";
        break;
    case AsInstr::Operands::Label:
        res += L", ";
        res += l2;
        break;
    case AsInstr::Operands::Const:
        res += L", ";
        res += L"$";
        res += l2;
        break;
    }

    return res + L"\n";
}

void AsInstr::SetSizeSuffix(size_t bytes)
{
    InstrSuffix is{};
    switch (bytes)
    {
    case 1:  is = InstrSuffix::s_b; break;
    case 2:  is = InstrSuffix::s_w; break;
    case 4:  is = InstrSuffix::s_l; break;
    case 8:  is = InstrSuffix::s_q; break;
    }

    if (suf == InstrSuffix::Last)
    {
        suf = is;
        return;
    }

    if (suf0 == InstrSuffix::Last)
    {
        suf0 = is;
        return;
    }

    suf1 = is;
}

void AsInstr::SwapOperands()
{
    std::swap(oper1, oper2);
    std::swap(l1, l2);
    std::swap(reg1, reg2);
    std::swap(mem1, mem2);
}

