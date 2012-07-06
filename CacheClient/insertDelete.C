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
//  This tool deletes a hot program address from the code cache
//      from an instrumentation routine (in the Jit).
//  Sample usage:
//    pin -t insertDelete -- /bin/ls

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int insertions, deletions, readyToDelete;
ADDRINT hotAddress;
ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "insertDelete.out", "specify trace file name");

/* ===================================================================== */
VOID InitCounters()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());

    insertions = 0;
    deletions = 0;
    readyToDelete = 0;
    hotAddress = 0x4000d8c0;
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID WatchTraces(TRACE trace, VOID *v)
{
    insertions++;
    ADDRINT orig_pc = TRACE_Address(trace);
    TraceFile << "Code cache insertion #" << dec << insertions;
    TraceFile << "\torig pc=0x" << hex << orig_pc;
    TraceFile << "\tcache pc=0x" << hex << TRACE_CodeCacheAddress(trace);
    TraceFile << "\ttrace size (original)=" << dec << TRACE_OriginalSize(trace);
    TraceFile << "\ttrace size (in cache)=" << dec << TRACE_CodeCacheSize(trace);
    TraceFile << " (BBL=" << dec << TRACE_NumBbl(trace);
    TraceFile << " Ins=" << dec << TRACE_NumIns(trace) << ")" << endl;

    // Make sure deletion only happens after the trace has been inserted
    if (orig_pc == hotAddress) readyToDelete = 1;

    // Delete trace after every 10 insertions, make sure we don't delete what
    //  we're currently inserting.
    if ((insertions % 10 == 0)  && (readyToDelete == 1) && (orig_pc != hotAddress))
    {
        int numRemoved = CODECACHE_InvalidateTraceAtProgramAddress( hotAddress);
        deletions += numRemoved;
        TraceFile << "DELETING " << dec << numRemoved << " traces at 0x" << hex << hotAddress;
        TraceFile << "\tTotal Deletions: " << dec << deletions << endl;
        readyToDelete = 0;
    }
}
/* ================================================================== */
/*
 Print details at the end of the run
*/
VOID PrintDetailsOnExit(INT32 code, VOID *v)
{
    int traces = CODECACHE_NumTracesInCache();
    int stubs = CODECACHE_NumExitStubsInCache();
    int compensation = traces - stubs;
    traces = traces - compensation; 

    TraceFile << endl << "------------------------" << endl;
    TraceFile << dec << traces << " traces" << endl;
    TraceFile << stubs << " exit stubs" << endl;
    // TraceFile << compensation << " compensations" << endl;
    // TraceFile << (traces+stubs+compensation) << " total regions in the code cache" << endl;
    TraceFile << endl << deletions << " deletions" << endl;
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
    CODECACHE_AddTraceInsertedFunction(WatchTraces, 0);

    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintDetailsOnExit, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
