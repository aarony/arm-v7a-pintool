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
//  This tool shows the result of limiting the trace size to a 
//      given number of basic blocks 
//  Sample usage:
//    pin -t bb_test -b 1 -- /bin/ls

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int numBBs;
int insertions;
ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "bb_test.out", "specify trace file name");
KNOB<UINT32> KnobBBLimit(KNOB_MODE_WRITEONCE, "pintool",
    "b", "0", "Maximum number of basic blocks per trace");

/* ================================================================== */
/*
 Change the max basic blocks
*/
VOID InitBBCount()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());
    
    insertions = 0;
    
    numBBs = KnobBBLimit.Value();
    ASSERTX(numBBs > 0);
    CODECACHE_ChangeMaxBblsPerTrace(numBBs);
    
    TraceFile << "Max BBs Set To: " << numBBs << endl;
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID WatchTraces(TRACE trace, VOID *v)
{
    insertions++;
    ADDRINT orig_pc = TRACE_Address(trace);
    TraceFile << "Code cache insertion #" << insertions;
    TraceFile << "\torig pc=0x" << hex << orig_pc;
    TraceFile << "\tcache pc=0x" << hex << TRACE_CodeCacheAddress(trace);
    string name = RTN_FindNameByAddress(orig_pc);
    if (name == "")
        name = "*noname*";
        
    TraceFile << "\ttrace size (orig)=" << TRACE_OriginalSize(trace);
    TraceFile << "\ttrace size (cache)=" << TRACE_CodeCacheSize(trace);
    TraceFile << " (BBL=" << TRACE_NumBbl(trace) << " Ins=" << TRACE_NumIns(trace) << ")";
    // TraceFile << "\t+stub size=" << stub_size;
    TraceFile << "\t+name=" << name << endl;
}

/* ================================================================== */
/*
 Print sizes
*/
VOID PrintFinalInfo(INT32 code, VOID *v)
{
    TraceFile << "Final trace count: " << CODECACHE_NumTracesInCache() << endl;
    TraceFile << "Final cache size: " << BytesToString(CODECACHE_MemUsed()) << endl;
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

    // Register a routine that gets called when the cache is first initialized
    CODECACHE_AddCacheInitFunction(InitBBCount, 0);

    // Register a routine that gets called when a trace is
    //  inserted into the codecache
    CODECACHE_AddTraceInsertedFunction(WatchTraces, 0);
    
    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintFinalInfo, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
