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
//  This tool watches when the control is about to enter the
//       code cache and tells the user
//  Sample usage:
//    To see the first execution of each trace only 
//    pin -t enter_exit -- /bin/ls  
//
//    Turn off linking to see all executions
//    pin -xyzzy -link_indirect 0 -link_pcrel 0 -link_return 0 -t enter_exit -- /bin/ls  

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int numJumpsIntoCodeCache;
int numJumpsOutofCodeCache;
ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "enter_exit.out", "specify trace file name");

/* ================================================================== */
VOID InitCounters()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());

    numJumpsIntoCodeCache = 0;
    numJumpsOutofCodeCache = 0;
}

/* ================================================================== */
/*
 Print details of jumps into code cache
*/
VOID WatchEnter(ADDRINT cache_pc)
{
    numJumpsIntoCodeCache++;
    TraceFile << "Jump into code cache #" << dec << numJumpsIntoCodeCache;
    TraceFile << "\tcache pc=0x" << hex << cache_pc << endl;
}
/* ================================================================== */
/*
 Print details of jumps out of code cache
*/
VOID WatchExit(ADDRINT cache_pc)
{
    numJumpsOutofCodeCache++;
    TraceFile << "Jump out of code cache #" << dec << numJumpsOutofCodeCache;
    TraceFile << "\tcache pc=0x" << hex << cache_pc << endl;
}

/* ================================================================== */
/*
 Print details at the end of the run
*/
VOID PrintDetailsOnExit(INT32 code, VOID *v)
{
    int traces = CODECACHE_NumTracesInCache();

    TraceFile << endl << "------------------------" << endl;
    TraceFile << dec << traces << " traces" << endl;
    TraceFile << numJumpsIntoCodeCache << " control transfers into code cache" << endl;
    TraceFile << numJumpsOutofCodeCache << " control transfers out of code cache" << endl;
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

    // Register a routine that gets called when we jump into 
    //  the code cache
    CODECACHE_AddCodeCacheEnteredFunction(WatchEnter, 0);

    // Register a routine that gets called when we jump out of  
    //  the code cache
    CODECACHE_AddCodeCacheExitedFunction(WatchExit, 0);

    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintDetailsOnExit, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
