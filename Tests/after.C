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

VOID Fun(VOID * ad, UINT32 c)
{
    fprintf(out, "%p: %d\n",ad, c);
}
    
VOID Trace(TRACE trace, VOID *v)
{
    INS head = BBL_InsHead(TRACE_BblHead(trace));
    
    INS_InsertCall(head, IPOINT_BEFORE, (AFUNPTR)Fun, IARG_INST_PTR, IARG_UINT32, 1, IARG_END);
    INS_InsertCall(head, IPOINT_BEFORE, (AFUNPTR)Fun, IARG_INST_PTR, IARG_UINT32, 2, IARG_END);
    if (INS_HasFallThrough(head))
    {
        INS_InsertCall(head, IPOINT_AFTER, (AFUNPTR)Fun, IARG_INST_PTR, IARG_UINT32, 3, IARG_END);
        INS_InsertCall(head, IPOINT_AFTER, (AFUNPTR)Fun, IARG_INST_PTR, IARG_UINT32, 4, IARG_END);
    }

    // Test fall through of a trace
    INS tail = BBL_InsTail(TRACE_BblTail(trace));
    if (INS_HasFallThrough(tail))
    {
        INS_InsertCall(tail, IPOINT_AFTER, (AFUNPTR)Fun, IARG_INST_PTR, IARG_UINT32, 10, IARG_END);
    }
}

int main(INT32 argc, CHAR **argv)
{
    out = fopen("after.out", "w");
    
    PIN_Init(argc, argv);
    
    TRACE_AddInstrumentFunction(Trace, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
