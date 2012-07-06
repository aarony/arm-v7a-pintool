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
// This tool counts the number of SMC traces whose code has been modified
// by the running application
//

#include <stdio.h>
#include <iostream>
#include <memory.h>
#include <string.h>
#include "pin.H"

// The running count of SMC traces
UINT64 smcCount = 0;


// This function is called before every trace is executed
VOID DoSmcCheck(VOID * traceAddr,
                VOID * traceCopyAddr,
                USIZE traceSize,
                CONTEXT * ctxP)
{
    if (memcmp(traceAddr, traceCopyAddr, traceSize) != 0)
    {
        smcCount++;
        free(traceCopyAddr);
        CODECACHE_InvalidateTraceAtProgramAddress((ADDRINT)traceAddr);
        PIN_ExecuteAt(ctxP);
    }
}

// Pin calls this function every time a new trace is encountered
VOID InstrumentTrace(TRACE trace, VOID *v)
{
    VOID *  traceAddr;
    VOID *  traceCopyAddr;
    USIZE traceSize;
    
    traceAddr = (VOID *)TRACE_Address(trace);

#if 0
    if (traceAddr < (void*)0xbf000000)
        return;
    fprintf(stderr,"Instrumenting trace at %p\n",traceAddr);
#endif
    
    traceSize = TRACE_Size(trace);
    traceCopyAddr = malloc(traceSize);

    if (traceCopyAddr != 0) 
    {
        memcpy(traceCopyAddr, traceAddr, traceSize);
        // Insert a call to DoSmcCheck before every trace
        TRACE_InsertCall(trace, IPOINT_BEFORE, (AFUNPTR)DoSmcCheck,   
                         IARG_PTR, traceAddr,
                         IARG_PTR, traceCopyAddr,
                         IARG_UINT32 , traceSize,
                         IARG_CONTEXT,
                         IARG_END);
    }
}

// This function is called when the application exits
// It prints the SMC trace counter
VOID Fini(INT32 code, VOID *v)
{
    std::cerr << "SMC Count = " << smcCount << endl;
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{

    // Initialize pin
    PIN_Init(argc, argv);

    // Register InstrumentTrace function
    TRACE_AddInstrumentFunction(InstrumentTrace, 0);

    // Register application exit call back
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}


