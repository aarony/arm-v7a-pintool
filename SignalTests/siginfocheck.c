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
 * This test tries to raise each type of synchronous signal and prints
 * out the siginfo.si_addr field.  The test can be run under Pin to test
 * whether that field has the same value when run natively.
 */

#include <signal.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/prctl.h>


static sigjmp_buf JumpBuffer;
static unsigned int TestNumber = 0;

static void handle(int, siginfo_t*, void*);

extern void DoSIGSEGV();
extern void DoSIGBUS();
extern void DoSIGILL();
extern void DoSIGFPE();
extern void DoSIGTRAP();


int main()
{
    struct sigaction sigact;

    sigact.sa_sigaction = handle;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGSEGV\n");
        return 1;
    }
    if (sigaction(SIGBUS, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGBUS\n");
        return 1;
    }
    if (sigaction(SIGILL, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGILL\n");
        return 1;
    }
    if (sigaction(SIGFPE, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGSEGV\n");
        return 1;
    }
    if (sigaction(SIGTRAP, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGTRAP\n");
        return 1;
    }

    sigsetjmp(JumpBuffer, 1);

    for (;;)
    {
        switch (TestNumber)
        {
        case 0:
            printf("Raising SIGSEGV ...\n");
            DoSIGSEGV();
            printf("Failed to raise signal\n");
            TestNumber++;
            break;

        case 1:
            printf("Raising SIGBUS ...\n");
            if (prctl(PR_SET_UNALIGN, PR_UNALIGN_SIGBUS) != 0)
                printf("Can't set unaligned\n");
            DoSIGBUS();
            printf("Failed to raise signal\n");
            TestNumber++;
            break;

        case 2:
            printf("Raising SIGILL ...\n");
            DoSIGILL();
            printf("Failed to raise signal\n");
            TestNumber++;
            break;

        case 3:
            printf("Raising SIGFPE ...\n");
            DoSIGFPE();
            printf("Failed to raise signal\n");
            TestNumber++;
            break;

        case 4:
            printf("Raising SIGTRAP ...\n");
            DoSIGTRAP();
            printf("Failed to raise signal\n");
            TestNumber++;
            break;

        default:
            return 0;
        }
    }
}

static void handle(int sig, siginfo_t* info, void* vctxt)
{
    printf("Got signal %d, si_addr is 0x%lx\n", sig, (unsigned long)info->si_addr);
    TestNumber++;
    siglongjmp(JumpBuffer, 1);
}
