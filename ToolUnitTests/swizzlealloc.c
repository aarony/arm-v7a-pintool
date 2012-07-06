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
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

void * const TargetPrefix = (void *)0xb0000000;
void * const SwizzlePrefix = (void *)0xb1000000;

#if defined(__cplusplus)
extern "C"
#endif
void MyFree(void * p)
{
    fprintf(stderr,"MyFree %p\n",p);
}

#if defined(__cplusplus)
extern "C"
#endif
void * MyAlloc()
{
    return TargetPrefix;
}

void * mmemcpy(void * to, const void * from, size_t n, void ** toout, void **fromout)
{
void * d0, *d1, *d2;
__asm__ __volatile__(
	"cld; rep ; movsl; testb $2,%b4; je 1f; movsw;"
	"1:testb $1,%b4;je 2f;"
	"movsb;"
	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2):"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from) : "memory");
*toout = d1;
*fromout = d2;
return (to);
}

void memindex(void * ad)
{
    void * ptr = 0;
    
    __asm__ __volatile__("movl $0,(%0,%1,1)" :: "r"(ptr),"r"(ad));
}


int n = 8;

int main()
{
    char buffer2[20];
    char buffer3[20];

#if defined(TARGET_MAC) || defined(TARGET_BSD)
    void * mm = mmap(TargetPrefix, getpagesize(), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0);
#else    
    void * mm = mmap(TargetPrefix, getpagesize(), PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
#endif
    
    void * al;
    void *toout,*fromout;

    fprintf(stderr, "Mmap %p\n",mm);

    if( mm != TargetPrefix )
    {
        fprintf(stderr, "did not get requested mmap region\n");
        return -1;
    }
    

    al = MyAlloc();
    
    fprintf(stderr, "alloc %p\n", al);

    ((char*)al)[0] = 1;

    fprintf(stderr, "al %d\n",((char*)al)[0]);
    
    buffer2[0] = 2;
    buffer3[0] = 3;

    
    mmemcpy(buffer2, al, n, &toout, &fromout);
    fprintf(stderr, "al %d toout - to %x from %p fromout %p\n", buffer2[0], (char*)toout - (char*)buffer2, al, fromout);

    mmemcpy(al, buffer3, n, &toout, &fromout);

    fprintf(stderr, "al %d to %p toout %p fromout - from %x\n",((char*)al)[0], al, toout, (char*)fromout - (char*)buffer3);
    
    memindex(al);
    
    MyFree(al);
    
    return 0;
}
    
    
