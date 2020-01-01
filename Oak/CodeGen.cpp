#include "CodeGen.h"

inline AsMnemonic CodeGen::TTypeToMnemonic(TokenType type)
{
    switch (type)
    {
    case TokenType::Assign:
        return AsMnemonic::as_mov;
    case TokenType::OperPlus:
        return AsMnemonic::as_add;
    case TokenType::OperMin:
        return AsMnemonic::as_sub;
    case TokenType::OperMul:
        return AsMnemonic::as_imul;
    case TokenType::OperPow:
        throw; // TODO
    case TokenType::OperDiv:
        return AsMnemonic::as_idiv;
    case TokenType::OperPCent:
        throw; // TODO
    case TokenType::OperInc:
        return AsMnemonic::as_inc;
    case TokenType::OperDec:
        return AsMnemonic::as_dec;
    case TokenType::OperLShift:
        return AsMnemonic::as_shl;
    case TokenType::OperRShift:
        return AsMnemonic::as_shr;
    case TokenType::OperBWAnd:
        return AsMnemonic::as_and;
    case TokenType::OperBWOr:
        return AsMnemonic::as_or;
    case TokenType::OperNot:
        return AsMnemonic::as_not;
    case TokenType::OperXor:
        return AsMnemonic::as_xor;
    default:
        throw;
    }
}

String CodeGen::GenLabel()
{
    static size_t count = 0;

    return String(L"L") + std::to_wstring(count++);
}

inline bool CodeGen::GetGlobal(const String& name)
{
    for (Var* g : globals)
    {
        if (g->name == name) return true;
    }

    return false;
}

inline int64_t CodeGen::GetLocal(Var* v)
{
    for (auto& vec : locals)
    {
        for (std::pair<Var*, int64_t>& p : vec)
        {
            if (v == p.first || v->name == p.first->name) return p.second;
        }
    }
    return 0;
}

wchar_t CodeGen::GenInsrtSuffix(size_t bytes)
{
    switch (bytes)
    {
    case 1:
        return L'b';
    case 2:
        return L'w';
    case 4:
        return L'l';
    case 8:
        return L'q';
    }
    throw;
}

AsInstr CodeGen::GenInstruction(AsMnemonic m, VisitRes dst, VisitRes src, VisitRes& f_dst)
{
    String res = L"", f_src = L"";
    size_t bytes = GetTypeSize(dst.var_type);

    wchar_t suffix = GenInsrtSuffix(bytes);

    String reg;
    switch (bytes)
    {
    case 1:
    {
        reg = L"al";
        break;
    }
    case 2:
    {
        reg = L"ax";
        break;
    }
    case 4:
    {
        reg = L"eax";
        break;
    }
    case 8:
    {
        reg = L"rax";
        break;
    }
    }

    if (src.type == VisitRes::vrt_const)
    {
        res += L"mov";
        res += suffix;
        res += L" $" + src.data + L", %" + reg + L"\n";
        src = f_dst = VisitRes(reg, VisitRes::vrt_reg, dst.var_type);
    }

    if (GetTypeSize(src.var_type) < GetTypeSize(dst.var_type))
    {
        throw Error(L"Cannot implicitly convert types\n");
    }

    switch (dst.type)
    {
    case VisitRes::vrt_const:
    {
        f_src = L"$" + dst.data;
        break;
    }
    case VisitRes::vrt_label_p:
    case VisitRes::vrt_label_v:
    {
        throw; // TODO
    }
    case VisitRes::vrt_reg:
    {
        f_src = L"%" + dst.data;
        break;
    }
    case VisitRes::vrt_var:
    {
        res += L"mov" + suffix + String(L" ");
        if (GetGlobal(dst.data))
        {
            res += dst.data + L", ";
        }
        else
        {
            int64_t loc = GetLocal(dst.var_ptr);
            // the syntax: <stack_offset>(%rbp)
            //    e.g. -8(%rbp)
            //    Stack offset is always negative
            res += loc;
            res += L"(%rbp), ";
        }
        switch (bytes)
        {
        case 1:
        {
            res += L"%al\n";
            f_src = L"%al";
            break;
        }
        case 2:
        {
            res += L"%ax\n";
            f_src = L"%ax";
            break;
        }
        case 4:
        {
            res += L"%eax\n";
            f_src = L"%eax";
            break;
        }
        case 8:
        {
            res += L"%rax\n";
            f_src = L"%rax";
            break;
        }
        }
        break;
    }
    default:
        break;
    }
    res += MnemonicStr[(int)m];
    res += suffix;
    res += L" " + f_src + L", ";
    if (src.type == VisitRes::vrt_reg)
    {
        res += L"%";
        res += src.data;
    }
    else
    {
        res += src.data;
    }

    f_dst = src;

    return res;
}

CodeGen::VisitRes CodeGen::VisitTree(ASTNode* n, std::vector<AsInstr>& res, int64_t stack_offset)
{
    switch (n->type)
    {
    case NodeType::Range:
    {

        break;
    }
    case NodeType::ConstLeaf:
    {
        ConstLeaf* c = (ConstLeaf*)n;
        return VisitRes(c->data.data, VisitRes::vrt_const, c->GetTypeKW());
    }
    case NodeType::VarLeaf:
    {
        VarLeaf* vl = (VarLeaf*)n;
        VisitRes res(vl->data->name, VisitRes::vrt_var, vl->GetTypeKW());
        res.var_ptr = vl->data;
        return res;
    }
    case NodeType::UnOper:
    {
        UnOp* op = (UnOp*)n;

        switch (op->oper.type)
        {
        case TokenType::Keyword:
        {
            if (op->oper.kw_type == Keyword::kw_asm)
            {
                res.push_back(static_cast<ConstLeaf*>(op->operand)->data.data);
                return VisitRes(L"");
            }
            else if (op->oper.kw_type == Keyword::kw_ret)
            {
                const auto opr = VisitTree(op->operand, res, stack_offset);

                if (opr.type == VisitRes::vrt_const)
                {
                    res.push_back(String(L"movq $") + opr.data + String(L", %rax"));
                }
                else if (opr.type == VisitRes::vrt_label_p || opr.type == VisitRes::vrt_label_v)
                {
                    res.push_back(String(L"leaq ") + opr.data + String(L", %rax"));
                }
            }
            return VisitRes(L"");
        }
        }
        return VisitRes(L"");
    }
    case NodeType::BinOper:
    {
        BinOp* op = (BinOp*)n;
        const VisitRes rl = VisitTree(op->l, res, stack_offset);
        const VisitRes rr = VisitTree(op->r, res, stack_offset);

        switch (op->oper.type)
        {
        case TokenType::Assign:
        {
            if (rr.type == VisitRes::vrt_label_p)
            {
                if (GetGlobal(rl.data))
                {
                    res.push_back(String(L"leaq ") + rr.data + L", %rax");
                    res.push_back(String(L"movq %rax, ") + rl.data + L"");
                }
                // return left operand of assignment (used for expressions like a = b = c)
                return rl;
            }
        }
        }

        if (IsBinaryOp(op->oper.type))
        {
            VisitRes f_dst;
            res.push_back(GenInstruction(TTypeToMnemonic(op->oper.type), rr, rl, f_dst));
            return f_dst;
        }

        return VisitRes(L"");
    }
    case NodeType::Func:
    {
        Lambda* f = (Lambda*)n;

        std::vector<AsInstr> body;
        const String lb = GenLabel();

        functions.push_back(lb + String(L":"));
        functions.push_back(fn_header);
        VisitBlock(f->def, functions, f->params);
        functions.push_back(fn_footer);

        return VisitRes(lb, VisitRes::vrt_label_p, Keyword::kw_fn); // return label to the function
    }
    case NodeType::Var:
    {
        Var* var = (Var*)n;
        
        if (flags & local_space)
        {
            size_t alloc_size = GetTypeSize(var->var_type);
            stack_offset -= alloc_size;
            locals[locals.size() - 1].push_back(std::make_pair(var, stack_offset));
        }
        else
        {
            globals.push_back(var);
        }
        return VisitRes(L"");
    }
    }
    return VisitRes(L"");
}

void CodeGen::VisitNSpace(Namespace* ns)
{
    std::vector<AsInstr> res;
    cur_ns = ns;
    VisitBlock(ns->block, res);
    result.insert(result.end(), res.begin(), res.end());
}

void CodeGen::VisitBlock(StatementBlock* bl, std::vector<AsInstr>& res)
{
    int64_t stack_offset = 0;
    // true if we just entered the local space
    const bool vv = bl->is_fn && ((flags & local_space) == 0);
    if (bl->is_fn)
    {
        flags |= local_space;
        locals.push_back({});
    }

    std::vector<ASTNode*>& block = bl->children;
    for (ASTNode* node : block)
    {
        const auto _ = VisitTree(node, res, stack_offset);
    }

    if (bl->is_fn)
    {
        locals.pop_back();
    }

    if (vv)
    {
        flags &= ~local_space;
    }
}

void CodeGen::VisitBlock(StatementBlock* bl, std::vector<AsInstr>& res, std::vector<Var*>& params)
{

    VisitBlock(bl, res);
}

CodeGen::CodeGen(AST& ast)
{
    m_ast = &ast;
}

void CodeGen::WriteCode(const String& path)
{
    stream = std::wofstream(path);

    for (Namespace* ns : m_ast->prog)
    {
        VisitNSpace(ns);
    }

    stream << L".data\n";
    for (Var* g : globals)
    {
        const size_t mult = (g->is_arr ? g->arr->GetSize() : 1);

        stream << g->name << L":\n.zero " << GetTypeSize(g->var_type) * mult << L"\n";
    }

    stream << L".text\n.globl main\n";
    for (auto& s : functions)
    {
        stream << s.text << L"\n";
    }

    stream << L"main:\n";
    for (auto& s : result)
    {
        stream << s.text << L"\n";
    }

    if (!GetGlobal(L"_main"))
    {
        throw Error(L"\nfn main not defined\n");
    }
    stream << LR"(    movq _main, %rax
    call *%rax
    ret
)";
}

