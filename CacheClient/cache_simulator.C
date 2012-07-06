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
//    pin -t cache_simulator -- /bin/ls

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
int stretches;
int multiplier;
int flushes;
int numJumpsIntoCodeCache;
int numJumpsOutofCodeCache;
int cacheblocks;
int inaccessible;
int insertions;
int numLinks;
int numUnlinks;
int totalTraces;
int totalStubs;
int totalCompensations;
long int totalTraceSize;
long int totalStubSize;
long int totalCompensationSize;
bool exponential;
ofstream TraceFile;

/* ================================================================== */
/* Command-Line Switches                                              */
/* ================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "cache_simulator.out", "specify trace file name");
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

    flushes = 0;
    numJumpsIntoCodeCache = 0;
    numJumpsOutofCodeCache = 0;
    numLinks = 0;
    numUnlinks = 0;
    insertions = 0;
    cacheblocks = 0;
    stretches = 0;
    multiplier = 2;
    exponential = KnobExponentialGrowth.Value();
    totalTraces = 0;
    totalStubs = 0;
    totalCompensations = 0;
    totalTraceSize = 0;
    totalStubSize = 0;
    totalCompensationSize = 0;
}

/* ================================================================== */
/*
  When notified by Pin that the cache is full, perform a flush and
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

    cout << " STRETCH! (" << dec << stretches;
    cout << ")\tcache: " << BytesToString(cacheSize);
    cout << "\tblock: " << BytesToString(blockSize) << endl;

    if (exponential) multiplier *= multiplier;
}

/* ================================================================== */
/*
  When notified by Pin that the cache is full, perform a flush and
  tell the user about it.
*/
VOID FlushOnFull(UINT32 trace_size, UINT32 stub_size)
{
    flushes++;
    CODECACHE_FlushCache();
    cout << " SWOOSH! (" << dec << flushes << ")" << endl;
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
 Print details of links as they are patched
*/
VOID WatchLinks(ADDRINT branch_pc, ADDRINT target_pc)
{
    numLinks++;
    TraceFile << "LINK #" << dec << numLinks;
    TraceFile << "\tcache pc=0x" << hex << branch_pc;
    TraceFile << " \tpatched to pc=0x" << hex << target_pc << endl;
}
/* ================================================================== */
/*
 Print details of links unpatched
*/
VOID WatchUnlinks(ADDRINT branch_pc, ADDRINT stub_pc)
{
    numUnlinks++;
    TraceFile << "UNLINK #" << dec << numUnlinks;
    TraceFile << "\tcache pc=0x" << hex << branch_pc;
    TraceFile << " \trestored to stub pc=0x" << hex << stub_pc << endl;
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID WatchTraces(TRACE trace, VOID *v)
{
    insertions++;
    TraceFile << "Code cache insertion #" << dec << insertions;
    TraceFile << "\torig pc=0x" << hex << TRACE_Address(trace);
    TraceFile << "\tcache pc=0x" << hex << TRACE_CodeCacheAddress(trace);

    totalTraces++;
    totalStubs+=TRACE_NumBbl(trace);
    totalTraceSize += TRACE_CodeCacheSize(trace);

    string name = RTN_FindNameByAddress(TRACE_Address(trace));
    if (name == "")
        name = "*noname*";
        
    TraceFile << "\tcache trace size=" << dec << TRACE_CodeCacheSize(trace);
    TraceFile << " (BBL=" << dec << TRACE_NumBbl(trace) << " Ins=" << TRACE_NumIns(trace) << ")";
    TraceFile << "\t+name=" << name << endl;
}

/* ================================================================== */
/*
 Print details at the end of the run
*/
VOID PrintDetailsOnExit(INT32 code, VOID *v)
{
    TraceFile << endl << "--------------------------------------------------" << endl;
    TraceFile << dec << insertions << " insertions into code cache" << endl;
    TraceFile << totalTraces << " traces\tavg size=" << (totalTraceSize / totalTraces) << endl;
    TraceFile << dec << numJumpsIntoCodeCache << " control transfers into code cache" << endl;
    TraceFile << numJumpsOutofCodeCache << " control transfers out of code cache" << endl;
    TraceFile << numLinks << " links set in the code cache" << endl;
    TraceFile << numUnlinks << " links removed from the code cache" << endl;
    TraceFile << "--------------------------------------------------" << endl;
    TraceFile << "Final cache size: ";
    int cacheSizeLimit = CODECACHE_CacheSizeLimit();
    if (cacheSizeLimit)
        TraceFile << BytesToString(cacheSizeLimit) << endl;
    else
        TraceFile << "UNLIMITED" << endl;
    TraceFile << BytesToString(CODECACHE_MemUsed()) << " used" << endl;
    TraceFile << BytesToString(CODECACHE_MemReserved()) << " reserved" << endl;
    TraceFile << BytesToString(inaccessible) << " inaccessible" << endl;
    TraceFile << "Final block size: " << BytesToString(CODECACHE_BlockSize()) << endl;
    TraceFile << cacheblocks << " cache blocks created" << endl;
    TraceFile << flushes << " flushes!" << endl;
    TraceFile << stretches << " stretches!" << endl;
    TraceFile << "#eof" << endl;
    TraceFile.close();
}

/* ================================================================== */
/*
  Print details of a cache allocation
*/
VOID CollectBlockInfo(UINT32 block_size )
{
    cacheblocks++;

    UINT32 bytesused = CODECACHE_MemUsed();
    UINT32 bytesreserved = CODECACHE_MemReserved();
    inaccessible = bytesreserved - bytesused - block_size; 

    TraceFile << "New Block: " << BytesToString(block_size) << "\t";
    TraceFile << "Total Cache Used: " << BytesToString(bytesused) << " / " << BytesToString(bytesreserved) << "\t\t";
    TraceFile << "Wasted (Fragmentation): " << BytesToString(inaccessible) << endl;
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
    
    // Register a routine that gets called when new cache blocks are formed
    CODECACHE_AddCacheBlockFunction(CollectBlockInfo, 0);

    // Register a routine that gets called when we jump into 
    //  the code cache
    CODECACHE_AddCodeCacheEnteredFunction(WatchEnter, 0);

    // Register a routine that gets called when we jump out of  
    //  the code cache
    CODECACHE_AddCodeCacheExitedFunction(WatchExit, 0);

    // Register a routine that gets called when a trace is 
    //  inserted into the codecache
    CODECACHE_AddTraceInsertedFunction(WatchTraces, 0);

    // Register a routine that gets called when patch a link in
    //  the code cache
    CODECACHE_AddTraceLinkedFunction(WatchLinks, 0);

    // Register a routine that gets called when we unpatch a link in
    //  the code cache
    CODECACHE_AddTraceUnlinkedFunction(WatchUnlinks, 0);

    // Register a routine that gets called every time the cache is full
    CODECACHE_AddFullCacheFunction(FlushOnFull, 0);
    //CODECACHE_AddFullCacheFunction(GrowOnFull, 0);
    
    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintDetailsOnExit, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}

