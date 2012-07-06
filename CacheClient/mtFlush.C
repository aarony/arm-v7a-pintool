/*BEGIN_LEGAL 
INTEL CONFIDENTIAL
Copyright 2002-2005 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code (Material) are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material may
contain trade secrets and proprietary and confidential information of
Intel Corporation and its suppliers and licensors, and is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intels prior express written permission.  No license
under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication,
inducement, estoppel or otherwise. Any license under such intellectual
property rights must be express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or
alter this notice or any other notice embedded in Materials by Intel
or Intels suppliers or licensors in any way.
END_LEGAL */
//
//  ORIGINAL_AUTHOR: Kim Hazelwood
//
//  This tool flushes the code cache after every five
//      threads are created. (Run it with threadWait.c)
//  Sample usage:
//    pin -mt -t mtFlush -- threadWait

#include <stdio.h>
#include "pin.H"

FILE * out;
PIN_LOCK lock;
int numThreads;
int numFlushesTriggered;
int numFlushesOccurred;

void InitializeMe()
{
    out = fopen("mtFlush.out", "w");
    numThreads = 1;
    numFlushesTriggered = 0;
    numFlushesOccurred = 0;
}

VOID ThreadBegin(UINT32 threadid, VOID * sp, int flags, VOID *v)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "thread begin %d\n",threadid);
    numThreads++;
    if (threadid % 5 == 0) 
    {
        numFlushesTriggered++;
        fprintf(out, "DUNK! (%d)\n",numFlushesTriggered);
        CODECACHE_FlushCache();
    }
    fflush(out);
    ReleaseLock(&lock);
}
    
VOID ThreadEnd(UINT32 threadid, INT32 code, VOID *v)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "thread end %d code %d\n",threadid, code);
    fflush(out);
    ReleaseLock(&lock);
}
    
VOID FlushHappened()
{
    numFlushesOccurred++;
    fprintf(out, "SWOOSH! (%d)\n",numFlushesOccurred);
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(out, "Fini: code %d\n", code);
    fclose(out);
}

int main(INT32 argc, CHAR **argv)
{
    InitLock(&lock);

    InitializeMe();
    
    PIN_Init(argc, argv);
    
    PIN_AddThreadBeginFunction(ThreadBegin, 0);
    PIN_AddThreadEndFunction(ThreadEnd, 0);
    CODECACHE_AddCacheFlushedFunction(FlushHappened, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}

