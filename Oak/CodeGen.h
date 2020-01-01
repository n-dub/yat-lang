#pragma once
#include "AST.h"
#include "Utils.h"
#include "CodeTemplates.h"

#pragma warning(disable : 26812)

// assembly instruction
struct AsInstr
{
    String text;
    AsInstr(String s)
    {
        text = s;
    }
};

class CodeGen
{
    std::vector<AsInstr> result;

    enum
    {
        // indicates that all new variables must be allocated
        // in the local space, i.e. on the stack
        local_space = 1 << 0,
        Last = ~0
    };

    int flags = 0;

    // all global variables
    std::vector<Var*> globals;

    // pairs variable : offset(address)
    std::vector<std::vector<std::pair<Var*, int64_t>>> locals;

    // all function bodies
    std::vector<AsInstr> functions;

    AST* m_ast = nullptr;
    Namespace* cur_ns = nullptr;
    std::wofstream stream;

    inline AsMnemonic TTypeToMnemonic(TokenType type);
    inline String GenLabel();
    inline bool GetGlobal(const String& name);
    inline int64_t GetLocal(Var* v);

    struct VisitRes
    {
        enum VisitResType
        {
            // label (pointer)
            vrt_label_p,
            // label (value)
            vrt_label_v,
            // name of a register
            vrt_reg,
            // name of a variable
            vrt_var,
            // constant
            vrt_const,
            // none
            vrt_none
        } type = vrt_none;

        VisitRes() = default;

        VisitRes(String s)
        {
            data = s;
        }

        VisitRes(String s, VisitResType type, Keyword dt)
        {
            this->type = type;
            data = s;
            var_type = dt;
        }

        String data;
        Var* var_ptr = nullptr;
        Keyword var_type = Keyword::Last;
    };

    wchar_t GenInsrtSuffix(size_t bytes);
    AsInstr GenInstruction(AsMnemonic m, VisitRes dst, VisitRes src, VisitRes& f_dst);
    VisitRes VisitTree(ASTNode* n, std::vector<AsInstr>& res, int64_t stack_offset);
    void VisitNSpace(Namespace* ns);
    void VisitBlock(StatementBlock* bl, std::vector<AsInstr>& res);
    void VisitBlock(StatementBlock* bl, std::vector<AsInstr>& res, std::vector<Var*>& params);
public:
    CodeGen(AST& ast);
    void WriteCode(const String& path);
};

