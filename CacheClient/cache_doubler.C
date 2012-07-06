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
//  This tool implements a custom code cache management policy
//  Sample usage:
//    pin -xyzzy -limit_code_cache 131072 -t cache_doubler -- /bin/ls

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int stretches, multiplier;
bool exponential;
ofstream TraceFile;

/* ================================================================== */
/* Command-Line Switches                                              */
/* ================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "cache_doubler.out", "specify trace file name");
KNOB<BOOL> KnobExponentialGrowth(KNOB_MODE_WRITEONCE, "pintool",
    "exp", "0", "Cache size grows exponentially");

/* ================================================================== */
/*
 Clear the cache flush count
*/
VOID InitStretchCount()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());
    
    stretches = 0;
    multiplier = 2;
    exponential = KnobExponentialGrowth.Value();
}

/* ================================================================== */
/*
 Print current flush count
*/
VOID PrintStretchInfo(INT32 code, VOID *v)
{
    TraceFile << endl << stretches << " stretches!" << endl;
    
    TraceFile << "Final cache size: " << BytesToString(CODECACHE_CacheSizeLimit()) << endl;
    TraceFile << "Final block size: " << BytesToString(CODECACHE_BlockSize()) << endl;
    TraceFile << "#eof" << endl;
    TraceFile.close();

    cout << "Cache Doubler Complete\n";
}

/* ================================================================== */
/*
  When notified by Pin that the cache is full, double the cache and
  tell the user about it.
*/
VOID GrowOnFull(UINT32 trace_size, UINT32 stub_size)
{
    stretches++;

    USIZE cacheSize = CODECACHE_CacheSizeLimit();
    USIZE blockSize = CODECACHE_BlockSize();

    cacheSize *= multiplier;
    if (stretches > 1) blockSize *= multiplier;

    CODECACHE_ChangeCacheLimit(cacheSize);
    CODECACHE_ChangeBlockSize(blockSize);
    ASSERTX(CODECACHE_CreateNewCacheBlock(blockSize));

    TraceFile << " STRETCH! (" << stretches;
    TraceFile << ")\tcache: " << BytesToString(cacheSize);
    TraceFile << "\tblock: " << BytesToString(blockSize) << endl;

    if (exponential) multiplier *= multiplier;
}

/* ================================================================== */
/*
 Initialize and begin program execution under the control of Pin
*/
int main(INT32 argc, CHAR **argv)
{
    if (PIN_Init(argc, argv)) return Usage();

    // Initialize some local data structures
    InitStretchCount();
    
    // Register a routine that gets called when the cache is first initialized
    CODECACHE_AddCacheInitFunction(PrintInitInfo, 0);
    
    // Register a routine that gets called every time the cache is full
    CODECACHE_AddFullCacheFunction(GrowOnFull, 0);
    
    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintStretchInfo, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
