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
//  This tool tells you what traces reappear after a code cache
//      flush. It can be used to identify long-lived traces in a program.
//      Note: you must limit the code cache size to trigger flushes
//  Sample usage:
//    pin -xyzzy -limit_code_cache 131072 -t flushAndInsert -- /bin/ls

#include "pin.H"
#include "utils.H"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int flushes;
int insertions;
ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "flushAndInsert.out", "specify trace file name");

/* ================================================================== */
/*
 Clear the cache flush count
*/
VOID InitCounters()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());

    insertions = 0;
    flushes = 0;
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID WatchTraces(TRACE trace, VOID *v)
{
    insertions++;
    TraceFile << "Code cache insertion #" << insertions;
    TraceFile << "\torig pc=0x" << hex << TRACE_Address(trace);
    TraceFile << "\tcache pc=0x" << hex << TRACE_CodeCacheAddress(trace);
    TraceFile << "\ttrace size (cache)=" << TRACE_CodeCacheSize(trace);
    TraceFile << " (BBL=" << TRACE_NumBbl(trace) << " Ins=" << TRACE_NumIns(trace) << ")" << endl;
}
/* ================================================================== */
/*
  When notified by Pin that the cache is full, perform a flush and
  tell the user about it.
*/
VOID FlushOnFull(UINT32 trace_size, UINT32 stub_size)
{
    //TraceFile << "Trying to insert trace size " << trace_size << " and exit stub size " << stub_size << "... ";
    flushes++;
    CODECACHE_FlushCache();
    TraceFile << " SWOOSH! (" << flushes << ")" << endl;
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
    TraceFile << compensation << " compensations" << endl;
    TraceFile << (traces+stubs+compensation) << " total regions in the code cache" << endl;
    TraceFile << endl << flushes << " flushes!" << endl;
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

    // Register a routine that gets called every time the cache is full
    CODECACHE_AddFullCacheFunction(FlushOnFull, 0);
    
    // Register a routine that gets called when a trace is 
    //  inserted into the codecache
    CODECACHE_AddTraceInsertedFunction(WatchTraces, 0);

    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintDetailsOnExit, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}

