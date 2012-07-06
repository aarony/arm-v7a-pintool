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
 * This test verifies that Pin properly saves and restores the applicaton's
 * XMM registers when emulating a synchronous signal (i.e. fault).  The
 * application's main thread does a memory copy operation using the XMM
 * registers.  However, the copy causes a fault by accessing an illegal memory
 * location.  A handler catches the fault and fixes the address of the illegal
 * memory location.  It also modifies the XMM registers.  If Pin doesn't
 * properly save/restore the XMM registers in the handler, the main thread's
 * memory copy will be corrupted.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define __USE_GNU
#include <sys/ucontext.h>

#define SIZE       32
#define ALIGN      16

char *SigBuf1;
char *SigBuf2;
unsigned long Glob;


static void XmmCheck();
static void CheckBuf(const char *, size_t);
static char *Allocate(size_t, size_t);
static void Handle(int, siginfo_t *, void *);


int main()
{
    struct sigaction sigact;
    int i;


    SigBuf1 = Allocate(SIZE, ALIGN);
    SigBuf2 = Allocate(SIZE, ALIGN);
    for (i = 0; i < SIZE;  i++)
        SigBuf1[i] = (char)i;

    sigact.sa_sigaction = Handle;
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGSEGV, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable to set up handler\n");
        return 1;
    }

    XmmCheck();
    return 0;
}


static void XmmCheck()
{
    char *p1;
    char *p2;
    int i;

    p1 = Allocate(SIZE, ALIGN);
    p2 = Allocate(SIZE, ALIGN);
    memset(p2, 0, SIZE);

    for (i = 0;  i < SIZE;  i++)
        p1[i] = "abcdefghijklmnopqrstuvwxyz"[i%26];

    /* This routine causes a fault by accessing an illegal memory location */
    CopyWithXmmFault(p2, p1, SIZE);

    /* Verify that the memory was copied correctly */
    CheckBuf(p2, SIZE);
}


static void CheckBuf(const char *p, size_t size)
{
    int i;
    char c;

    for (i = 0;  i < size;  i++)
    {
        c = "abcdefghijklmnopqrstuvwxyz"[i%26];
        if (p[i] != c)
        {
            fprintf(stderr, "Element %d wrong: is '%c' should be '%c'\n", i, p[i], c);
            exit(1);
        }
    }
}


static char *Allocate(size_t size, size_t align)
{
    char *p;
    size_t low;

    p = malloc(size + (align-1));
    low = (size_t)p % align;
    if (low)
        return p + (align-low);
    else
        return p;
}


static void Handle(int sig, siginfo_t *i, void *vctxt)
{
    ucontext_t *ctxt = vctxt;

    /* Fix the illegal memory address access */
#if defined(TARGET_IA32)
    ctxt->uc_mcontext.gregs[REG_EAX] = (unsigned long)&Glob;
#elif defined(TARGET_IA32E)
    ctxt->uc_mcontext.gregs[REG_RAX] = (unsigned long)&Glob;
#endif

    /* This changes the values of the XMM registers */
    CopyWithXmm(SigBuf2, SigBuf1, SIZE);
}
