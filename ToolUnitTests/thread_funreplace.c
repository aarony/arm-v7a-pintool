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
#include <sched.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>

int addnums(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    int result;
    
    //printf("addnums(): n0=%d n1=%d n2=%d n3=%d n4=%d n5=%d n6=%d n7=%d n8=%d n9=%d\n",
    //       n0, n1, n2, n3, n4, n5, n6, n7, n8, n9);
    
    result = n0+n1+n2+n3+n4+n5+n6+n7+n8+n9;
    return result;
}

void * foo(void * arg)
{
    int x = (int)arg;
    
    addnums(x, x+1, x+2, x+3, x+4, x+5, x+6, x+7, x+8, x+9);
    
    return 0;
}

#define MAXTHREADS 1000

int threads_started;

int main(int argc, char *argv[])
{
    int numthreads = 0;
    int i;
    pthread_t threads[MAXTHREADS];
    
    numthreads = 4;
    assert(numthreads < MAXTHREADS);
    
    for (threads_started = 0; threads_started < numthreads; threads_started++)
    {
        printf("Creating thread\n");
        fflush(stdout);
        pthread_create(threads+threads_started, 0, foo, (void*)threads_started);
        fflush(stdout);
    }

    for (i = 0; i < numthreads; i++)
    {
        pthread_join(threads[i], 0);
        printf("Joined %d\n", i);
    }
    printf("All threads joined\n");

    return 0;
}
