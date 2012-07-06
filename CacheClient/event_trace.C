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
//  ORIGINAL_AUTHOR: Robert Muth
//
//  This tool watches various code cache activities 
//       and dumps them to a file
//  Sample usage:
//    pin -t event_trace -- /bin/ls

#include "pin.H"

#include <fstream>
#include <iostream>

using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
ofstream    OutFile ("ctrace.out");
int numTraces;

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/

VOID WatchTraces(TRACE trace, VOID *v)
{
    ADDRINT orig_pc = TRACE_Address(trace);
    string name = RTN_FindNameByAddress(orig_pc);
    if (name == "")
        name = "*noname*";
       
    OutFile << "@ITRACE " << dec
            << numTraces++
            << " 0x" << hex << orig_pc
            << " 0x" << hex << TRACE_CodeCacheAddress(trace) << " " << dec 
            << TRACE_NumBbl(trace) << " " << dec 
            << TRACE_NumIns(trace) << " " << dec 
            << TRACE_OriginalSize(trace) << " " << dec 
            << TRACE_CodeCacheSize(trace) << " " 
            << name
            << endl << flush;
}

/* ================================================================== */
/*

*/

VOID WatchLinks(ADDRINT branch_pc, ADDRINT target_pc)
{
    OutFile << "@LINK "
            << " 0x" << hex << branch_pc
            << " 0x" << hex << target_pc
            << endl << flush;
}


/* ================================================================== */
/*
 Initialize and begin program execution under the control of Pin
*/
int main(INT32 argc, CHAR **argv)
{
    numTraces = 0;

    PIN_InitSymbols();

    PIN_Init(argc, argv);

    CODECACHE_AddTraceInsertedFunction(WatchTraces, 0);

    CODECACHE_AddTraceLinkedFunction(WatchLinks, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}

/* ================================================================== */
