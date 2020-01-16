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

Register CvtReg(Register r, size_t ns);

