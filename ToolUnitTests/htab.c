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
// This test looks for fails in updating the hash table
// Even with an intentional bug, it rarely fails
// Increase TRIES to make it run longer and more likely to find the bug

#define TRIES 1000

#include <sched.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>

int a[100000];
int n = 10;

typedef int (*FUNPTR)();

#define FUN(n) int fun##n() { return n; }
#define FUN10(n) \
FUN(n##0) \
FUN(n##1) \
FUN(n##2) \
FUN(n##3) \
FUN(n##4) \
FUN(n##5) \
FUN(n##6) \
FUN(n##7) \
FUN(n##8) \
FUN(n##9)

#define FUN100(n) \
FUN10(n##0) \
FUN10(n##1) \
FUN10(n##2) \
FUN10(n##3) \
FUN10(n##4) \
FUN10(n##5) \
FUN10(n##6) \
FUN10(n##7) \
FUN10(n##8) \
FUN10(n##9)

FUN100(1)
FUN100(2)
FUN100(3)
FUN100(4)
FUN100(5)
FUN100(6)
FUN100(7)
FUN100(8)
FUN100(9)
FUN100(10)
FUN100(11)
FUN100(12)
FUN100(13)

#define FUNINIT10(n) fun##n##0,fun##n##1,fun##n##2,fun##n##3,fun##n##4,fun##n##5,fun##n##6,fun##n##7,fun##n##8,fun##n##9
#define FUNINIT100(n) FUNINIT10(n##0),FUNINIT10(n##1),FUNINIT10(n##2),FUNINIT10(n##3),FUNINIT10(n##4),FUNINIT10(n##5),FUNINIT10(n##6),FUNINIT10(n##7),FUNINIT10(n##8),FUNINIT10(n##9)

FUNPTR funs[] = 
{
    FUNINIT100(1),
    FUNINIT100(2),
    FUNINIT100(3),
    FUNINIT100(4),
    FUNINIT100(5),
    FUNINIT100(6),
    FUNINIT100(7),
    FUNINIT100(8),
    FUNINIT100(9),
    FUNINIT100(10),
    FUNINIT100(11),
    FUNINIT100(12),
    FUNINIT100(13)
};

    
void * hello(void * arg)
{
    int i;
    int j;
    
    for (j = 0; j < TRIES; j++)
    {
        for (i = 0; i < 1300; i++)
        {
            int res = funs[i]();
            int expect = i + 100;
            

            if (res != expect)
            {
                printf("Expected %d, got %d\n",expect, res);
                exit(1);
            }
        }
    }

    return 0;
}
        
#define MAXTHREADS 1000

int threads_started;

int main(int argc, char *argv[])
{
    int numthreads = 0;
    int i;
    pthread_t threads[MAXTHREADS];
    
    numthreads = 2;
    assert(numthreads < MAXTHREADS);
    
    for (threads_started = 0; threads_started < numthreads; threads_started++)
    {
        printf("Creating thread\n");
        fflush(stdout);
        pthread_create(threads+threads_started, 0, hello, 0);
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
