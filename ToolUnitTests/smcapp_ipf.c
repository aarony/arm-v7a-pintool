/*BEGIN_LEGAL 
#BEGIN_LEGAL
##BEGIN_LEGAL
##INTEL CONFIDENTIAL
##Copyright 2002-2005 Intel Corporation All Rights Reserved.
##
##The source code contained or described herein and all documents
##related to the source code (Material) are owned by Intel Corporation
##or its suppliers or licensors. Title to the Material remains with
##Intel Corporation or its suppliers and licensors. The Material may
##contain trade secrets and proprietary and confidential information of
##Intel Corporation and its suppliers and licensors, and is protected by
##worldwide copyright and trade secret laws and treaty provisions. No
##part of the Material may be used, copied, reproduced, modified,
##published, uploaded, posted, transmitted, distributed, or disclosed in
##any way without Intels prior express written permission.  No license
##under any patent, copyright, trade secret or other intellectual
##property right is granted to or conferred upon you by disclosure or
##delivery of the Materials, either expressly, by implication,
##inducement, estoppel or otherwise. Any license under such intellectual
##property rights must be express and approved by Intel in writing.
##
##Unless otherwise agreed by Intel in writing, you may not remove or
##alter this notice or any other notice embedded in Materials by Intel
##or Intels suppliers or licensors in any way.
##END_LEGAL
#INTEL CONFIDENTIAL
#Copyright 2002-2005 Intel Corporation All Rights Reserved.
#
#The source code contained or described herein and all documents
#related to the source code (Material) are owned by Intel Corporation
#or its suppliers or licensors. Title to the Material remains with
#Intel Corporation or its suppliers and licensors. The Material may
#contain trade secrets and proprietary and confidential information of
#Intel Corporation and its suppliers and licensors, and is protected by
#worldwide copyright and trade secret laws and treaty provisions. No
#part of the Material may be used, copied, reproduced, modified,
#published, uploaded, posted, transmitted, distributed, or disclosed in
#any way without Intels prior express written permission.  No license
#under any patent, copyright, trade secret or other intellectual
#property right is granted to or conferred upon you by disclosure or
#delivery of the Materials, either expressly, by implication,
#inducement, estoppel or otherwise. Any license under such intellectual
#property rights must be express and approved by Intel in writing.
#
#Unless otherwise agreed by Intel in writing, you may not remove or
#alter this notice or any other notice embedded in Materials by Intel
#or Intels suppliers or licensors in any way.
#END_LEGAL
Intel Open Source License 

Copyright (c) 2002-2005 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
/*
 * This is a simple SMC (self-modifying-code) test.  This application uses
 * SMC to call Sub1() and then Sub2().
 */

#include <stdio.h>

typedef unsigned long UINT64;

extern void Call();
extern void FlushICache(void *);

static void Sub1();
static void Sub2();
static UINT64 GetImm();
static void PutImm(UINT64);
static UINT64 *GetMovlPtr();
static UINT64 GetFuncAddr(void (*)());


int main()
{
    PutImm(GetFuncAddr(&Sub1));
    Call();

    PutImm(GetFuncAddr(&Sub2));
    Call();

    return 0;
}


static void Sub1()
{
    printf("In Sub1\n");
}

static void Sub2()
{
    printf("In Sub2\n");
}

static UINT64 GetImm()
{
    UINT64 lo;
    UINT64 hi;
    UINT64 i;
    UINT64 imm9d;
    UINT64 imm5c;
    UINT64 ic;
    UINT64 imm7b;
    UINT64 imm41;
    UINT64 val;
    UINT64 *callmovl;

    callmovl = GetMovlPtr();
    lo = callmovl[0];
    hi = callmovl[1];

    i = (hi >> 59) & 1;
    imm41 = ((hi & 0x7fffff) << 18) | ((lo >> 46) & 0x3ffff);
    ic = (hi >> 44) & 1;
    imm5c = (hi >> 45) & 0x1f;
    imm9d = (hi >> 50) & 0x1ff;
    imm7b = (hi >> 36) & 0x7f;

    val = (i << 63) | (imm41 << 22) | (ic << 21) | (imm5c << 16) | (imm9d << 7) | imm7b;
    return val;
}

static void PutImm(UINT64 val)
{
    UINT64 lo;
    UINT64 hi;
    UINT64 i;
    UINT64 imm9d;
    UINT64 imm5c;
    UINT64 ic;
    UINT64 imm7b;
    UINT64 imm41;
    UINT64 *callmovl;

    i = (val >> 63) & 1;
    imm41 = (val >> 22) & 0x1ffffffffff;
    ic = (val >> 21) & 1;
    imm5c = (val >> 16) & 0x1f;
    imm9d = (val >> 7) & 0x1ff;
    imm7b = val & 0x7f;

    callmovl = GetMovlPtr();
    lo = callmovl[0];
    hi = callmovl[1];

    hi = (hi & ~(1ul<<59)) | (i << 59);
    hi = (hi & ~(0x1fful<<50)) | (imm9d << 50);
    hi = (hi & ~(0x1ful<<45)) | (imm5c << 45);
    hi = (hi & ~(1ul<<44)) | (ic << 44);
    hi = (hi & ~(0x7ful<<36)) | (imm7b << 36);
    hi = (hi & ~0x7ffffful) | (imm41 >> 18);
    lo = (lo & ~(0x3fffful<<46)) | ((imm41 & 0x3fffful) << 46);

    callmovl[0] = lo;
    callmovl[1] = hi;

    FlushICache(callmovl);
}

static UINT64 *GetMovlPtr()
{
    UINT64 val;

    val = GetFuncAddr(&Call);
    return (UINT64*)(val+16);
}

static UINT64 GetFuncAddr(void (*pf)())
{
    UINT64 *ptr;
    UINT64 val;

    ptr = (UINT64*)pf;
    val = *ptr;
    return val;
}
