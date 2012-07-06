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
//    pin -xyzzy -limit_code_cache 131072 -cache_policy custom -t cache_flusher -- /bin/ls

#include "pin.H"
#include "utils.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ================================================================== */
/* Global Data Structures                                             */
/* ================================================================== */

/* ================================================================== */
int flushes;
ofstream SwooshFile;

/* ================================================================== */
/* Command-Line Switches                                              */
/* ================================================================== */
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "cache_flusher.out", "specify trace file name");

/* ================================================================== */
/*
 Clear the cache flush count
*/
VOID InitFlushCount()
{
    string logFileName = KnobOutputFile.Value();
    SwooshFile.open(logFileName.c_str());
    flushes = 0;
}

/* ================================================================== */
/*
 Print current flush count
*/
VOID PrintFlushInfo(INT32 code, VOID *v)
{
    cout << endl << flushes << " flushes!" << endl;
    SwooshFile << "#eof" << endl;
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
    SwooshFile << " SWOOSH! (" << flushes << ")" << endl;
}

/* ================================================================== */
/*
 Initialize and begin program execution under the control of Pin
*/
int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);

    // Initialize some local data structures
    InitFlushCount();
    
    // Register a routine that gets called when the cache is first initialized
    CODECACHE_AddCacheInitFunction(PrintInitInfo, 0);
    
    // Register a routine that gets called every time the cache is full
    CODECACHE_AddFullCacheFunction(FlushOnFull, 0);
    
    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(PrintFlushInfo, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
