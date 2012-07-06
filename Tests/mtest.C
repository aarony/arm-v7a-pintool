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
#include <stdio.h>
#include "pin.H"

FILE * out;
PIN_LOCK lock;

VOID ThreadBegin(UINT32 threadid, VOID * sp, int flags, VOID *v)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "thread begin %d sp %p flags %x\n",threadid, sp, flags);
    ReleaseLock(&lock);
}
    
VOID ThreadEnd(UINT32 threadid, INT32 code, VOID *v)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "thread end %d code %d\n",threadid, code);
    ReleaseLock(&lock);
}
    
VOID TraceBegin(VOID * ip, UINT32 threadid)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "%p: %d\n", ip, threadid);
    ReleaseLock(&lock);
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(out, "Fini: code %d\n", code);
}

VOID Trace(TRACE trace, VOID *v)
{
    TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(TraceBegin), IARG_INST_PTR, IARG_THREAD_ID, IARG_END);
}

int main(INT32 argc, CHAR **argv)
{
    InitLock(&lock);
    
    out = fopen("mt.out", "w");
    
    PIN_Init(argc, argv);
    
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddThreadBeginFunction(ThreadBegin, 0);
    PIN_AddThreadEndFunction(ThreadEnd, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
