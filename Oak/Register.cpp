#include "Utils.h"
#include "Register.h"

const String RegisterStr[]{
    L"al",
    L"bl",
    L"cl",
    L"dl",
    L"ah",
    L"bh",
    L"ch",
    L"dh",
    L"ax",
    L"bx",
    L"cx",
    L"dx",
    L"eax",
    L"ebx",
    L"ecx",
    L"edx",
    L"rax",
    L"rbx",
    L"rcx",
    L"rdx",
    L"rbp",
    L"rsp",
    L"rsi",
    L"rdi",
    L"r8",
    L"r9",
    L"r10",
    L"r11",
    L"r12",
    L"r13",
    L"r14",
    L"r15",
    L"r8d",
    L"r9d",
    L"r10d",
    L"r11d",
    L"r12d",
    L"r13d",
    L"r14d",
    L"r15d",
    L"r8w",
    L"r9w",
    L"r10w",
    L"r11w",
    L"r12w",
    L"r13w",
    L"r14w",
    L"r15w",
    L"r8b",
    L"r9b",
    L"r10b",
    L"r11b",
    L"r12b",
    L"r13b",
    L"r14b",
    L"r15b",
    L"xmm0",
    L"xmm1",
    L"xmm2",
    L"xmm3",
    L"xmm4",
    L"xmm5",
    L"xmm6",
    L"xmm7",
    L"xmm8",
    L"xmm9",
    L"xmm10",
    L"xmm11",
    L"xmm12",
    L"xmm13",
    L"xmm14",
    L"xmm15",
    L"Last"
};

Register CvtReg(Register r, size_t ns)
{
    switch (r)
    {
    case Register::al:
    case Register::ax:
    case Register::eax:
    case Register::rax:
        switch (ns)
        {
        case 1: return Register::al;
        case 2: return Register::ax;
        case 4: return Register::eax;
        case 8: return Register::rax;
        }
        break;
    case Register::bl:
    case Register::bx:
    case Register::ebx:
    case Register::rbx:
        switch (ns)
        {
        case 1: return Register::bl;
        case 2: return Register::bx;
        case 4: return Register::ebx;
        case 8: return Register::rbx;
        }
        break;
    case Register::cl:
    case Register::cx:
    case Register::ecx:
    case Register::rcx:
        switch (ns)
        {
        case 1: return Register::cl;
        case 2: return Register::cx;
        case 4: return Register::ecx;
        case 8: return Register::rcx;
        }
        break;
    case Register::dl:
    case Register::dx:
    case Register::edx:
    case Register::rdx:
        switch (ns)
        {
        case 1: return Register::dl;
        case 2: return Register::dx;
        case 4: return Register::edx;
        case 8: return Register::rdx;
        }
        break;
    case Register::r8:
    case Register::r8b:
    case Register::r8w:
    case Register::r8d:
        switch (ns)
        {
        case 1: return Register::r8b;
        case 2: return Register::r8w;
        case 4: return Register::r8d;
        case 8: return Register::r8;
        }
        break;
    case Register::r9:
    case Register::r9b:
    case Register::r9w:
    case Register::r9d:
        switch (ns)
        {
        case 1: return Register::r9b;
        case 2: return Register::r9w;
        case 4: return Register::r9d;
        case 8: return Register::r9;
        }
        break;
    case Register::r10:
    case Register::r10b:
    case Register::r10w:
    case Register::r10d:
        switch (ns)
        {
        case 1: return Register::r10b;
        case 2: return Register::r10w;
        case 4: return Register::r10d;
        case 8: return Register::r10;
        }
        break;
    case Register::r11:
    case Register::r11b:
    case Register::r11w:
    case Register::r11d:
        switch (ns)
        {
        case 1: return Register::r11b;
        case 2: return Register::r11w;
        case 4: return Register::r11d;
        case 8: return Register::r11;
        }
        break;
    case Register::r12:
    case Register::r12b:
    case Register::r12w:
    case Register::r12d:
        switch (ns)
        {
        case 1: return Register::r12b;
        case 2: return Register::r12w;
        case 4: return Register::r12d;
        case 8: return Register::r12;
        }
        break;
    case Register::r13:
    case Register::r13b:
    case Register::r13w:
    case Register::r13d:
        switch (ns)
        {
        case 1: return Register::r13b;
        case 2: return Register::r13w;
        case 4: return Register::r13d;
        case 8: return Register::r13;
        }
        break;
    case Register::r14:
    case Register::r14b:
    case Register::r14w:
    case Register::r14d:
        switch (ns)
        {
        case 1: return Register::r14b;
        case 2: return Register::r14w;
        case 4: return Register::r14d;
        case 8: return Register::r14;
        }
        break;
    case Register::r15:
    case Register::r15b:
    case Register::r15w:
    case Register::r15d:
        switch (ns)
        {
        case 1: return Register::r15b;
        case 2: return Register::r15w;
        case 4: return Register::r15d;
        case 8: return Register::r15;
        }
        break;
    }
}

