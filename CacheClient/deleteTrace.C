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
//  This tool deletes a hot program addresses from the code cache
//      from inside an analysis routine.
//  Sample usage:
//    pin -t deleteTrace -- /bin/ls

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
UINT32 insertions, deletions;
ADDRINT hotAddress;
UINT32 * executionCount;
ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "deleteTrace.out", "specify trace file name");


/* ================================================================== */
VOID InitCounters()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());

    insertions = 0;
    deletions = 0;
    hotAddress = 0x4000d8c0;
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID PrintImportantInsertions(TRACE trace, VOID *v)
{
    insertions++;

    ADDRINT orig_pc = TRACE_Address(trace);
    if (orig_pc == hotAddress) 
    {
        TraceFile << "INSERTION #" << dec << insertions;
        TraceFile << "\torig pc=0x" << hex << orig_pc;
        TraceFile << "\tcache pc=0x" << hex << TRACE_CodeCacheAddress(trace);
        TraceFile << "\ttrace size (orig)=" << TRACE_OriginalSize(trace);
        TraceFile << "\ttrace size (cache)=" << dec << TRACE_CodeCacheSize(trace);
        TraceFile << " (BBL=" << dec <<  TRACE_NumBbl(trace);
        TraceFile << " Ins=" << dec << TRACE_NumIns(trace) << ")" << endl;
        // TraceFile << "\t+stub size=" << stub_size << endl;
    }
}

/* ================================================================== */
/*
 Print details of traces as they are deleted
*/
VOID PrintImportantInvalidations(ADDRINT orig_pc, ADDRINT cache_pc,
    BOOL success)
{
    if (success) 
    {
        deletions++;
        TraceFile << "   SUCCESSFUL INVALIDATION #" << dec << deletions;
        TraceFile << "\torig pc=0x" << hex << orig_pc;
        TraceFile << "\tcache pc=0x" << hex << cache_pc << endl;
    }
    else
    {
        TraceFile << "   FAILED INVALIDATION";
        TraceFile << "\torig pc=0x" << hex << orig_pc;
        TraceFile << "\tcache pc=0x" << hex << cache_pc << endl;
    }
}

VOID InvalidateTrace()
{
    TraceFile << "INVALIDATION of 0x" << hex << hotAddress << " initiated" << endl;
    
    UINT32 numRemoved = CODECACHE_InvalidateTraceAtProgramAddress( hotAddress);
    
    TraceFile << "INVALIDATION " << dec << numRemoved << " traces at 0x" << hex << hotAddress;
    TraceFile << "\tTotal Invalidations: " << dec << deletions << endl;
}

VOID CheckForInvalidateTrace(UINT32 *theCount)
{
    (*theCount)++;
    if (*theCount > 50) 
    {
        cerr << "[ERROR] I should have been deleted!" << endl;
    }
    if (*theCount > 20) 
    {
        InvalidateTrace();
        (*theCount) = 0;
    }
}

VOID CheckForHotAddress(TRACE Trace, VOID *v)
{
    if (TRACE_Address(Trace) == hotAddress)
    {
        executionCount = new UINT32;
        (*executionCount) = 0;

        TRACE_InsertCall(Trace, IPOINT_BEFORE, (AFUNPTR) CheckForInvalidateTrace, IARG_PTR, executionCount, IARG_END);
    }
}

/* ================================================================== */
/*
 Print details at the end of the run
*/
VOID PrintDetailsOnExit(INT32 code, VOID *v)
{
    TraceFile << endl << dec << deletions << " deletions" << endl;
    TraceFile << "#eof" << endl;
    TraceFile.close();
}

/* ================================================================== */
/*
 Initialize and begin program execution under the control of Pin
*/
int main(INT32 argc, CHAR **argv)
{
    if (PIN_Init(argc, argv)) return Usage();

    // Initialize some counters we're using in this tool
    InitCounters();

    // Register a routine that gets called when the cache is first initialized
    CODECACHE_AddCacheInitFunction(PrintInitInfo, 0);

    // Register a routine that gets called when a trace is 
    //  inserted into the codecache
    CODECACHE_AddTraceInsertedFunction(PrintImportantInsertions, 0);

    // Register a routine that gets called when a trace is 
    //  inserted into the codecache
    CODECACHE_AddTraceInvalidatedFunction(PrintImportantInvalidations, 0);

    // Register an instrumentation routine that counts traces and evicts the
    // hot trace after 50 executions.
    TRACE_AddInstrumentFunction(CheckForHotAddress, 0);

    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintDetailsOnExit, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
