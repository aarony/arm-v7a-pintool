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
 * This test verifies that a Pin tool can intercept a synchronous signal
 * (i.e. fault) raised by the application and change the instrumentation
 * for the code that generated the fault.  This test is based on the
 * "swizzle5" tool that Robert Cohn wrote for Oracle.
 *
 * This test application runs correctly without a Pin tool, but it's only
 * interesting when run under the "swizzletool" tool.  The tool changes
 * return value from Allocate(), so that the application receives an
 * illegal addres instead of the expected buffer pointer.  The application
 * then generates a SIGSEGV whenever it attempts to access this buffer.
 * The tool catches this signal and adds instrumentation to the faulting
 * instruction that changes the illegal buffer address back into a legal
 * address.  The end result is that the application has the same effect
 * when run under the tool.
 *
 * Aside from generally testing Pin's ability to intercept a signal, this
 * test checks the following cases:
 *
 * 1) The application uses XMM registers to perform a memory copy, and the
 *    memory copy algorithm generates a SEGV that the tool intercepts.  This
 *    verifies that Pin properly saves and restores the application's XMM
 *    registers when the tool intercepts a signal.  (If Pin did not save
 *    and restore them properly, the memory copy would be corrupted, and
 *    this test would fail.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE    1024
#define ALIGN   16


static void CheckBuf(char *, size_t);
static CheckZeroBuf(char *, size_t);
char *Allocate(size_t, size_t);
void CopyWithXmm(char *, const char *, size_t);


int main()
{
    char *p1;
    char *p2;
    int i;

    p1 = Allocate(SIZE, ALIGN);
    p2 = Allocate(SIZE, ALIGN);
    printf("p1 = %p, p2 = %p\n", p1, p2);

    for (i = 0;  i < SIZE;  i++)
        p1[i] = "abcdefghijklmnopqrstuvwxyz"[i%26];

    memcpy(p2, p1, SIZE);
    CheckBuf(p2, SIZE);

    memset(p2, 0, SIZE);
    CheckZeroBuf(p2, SIZE);

    /*
     * Do a memory copy using XMM registers.  Verifies that Pin properly
     * saves and restores XMM registers when the SEGV is intercepted.
     */
    CopyWithXmm(p2, p1, SIZE);
    CheckBuf(p2, SIZE);

    return 0;
}


static void CheckBuf(char *p, size_t size)
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


static CheckZeroBuf(char *p, size_t size)
{
    int i;

    for (i = 0;  i < size;  i++)
    {
        if (p[i] != '\0')
        {
            fprintf(stderr, "Element %d not zero ('%c')\n", i, p[i]);
            exit(1);
        }
    }
}


/*
 * The tool instruments this routine and changes the return value to be an
 * invalid address.
 */
char *Allocate(size_t size, size_t align)
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
