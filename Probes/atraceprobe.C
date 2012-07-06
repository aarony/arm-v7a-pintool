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

/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Robert Cohn
*/

/* ===================================================================== */
/*! @file

  Demonstrates low cost instrumentation by selectively instrumenting the program.
  The uninstrumented parts of the program execution the original code and have no
  overhead. The application transitions to instrumented code by calling PIN_ExecuteInstrumented.
  This function must be passed a context, which is allocated by the application by calling
  PIN_NewThread().
  The application transitions back to original code by calling PIN_ExecuteUninstrumented. See an
  example application in mttraceapp.c.

 */



#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile("atraceprobe.out");

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool collects an address trace\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

pthread_mutex_t iomutex = PTHREAD_MUTEX_INITIALIZER;

// Print a memory read record
VOID RecordMemRead(UINT32 id, VOID * ip, VOID * addr)
{
    pthread_mutex_lock(&iomutex);
    
    TraceFile << "T" << id << " " << ip << ": R " << addr << endl;

    pthread_mutex_unlock(&iomutex);
}

// Print a memory write record
VOID RecordMemWrite(UINT32 id, VOID * ip, VOID * addr)
{
    pthread_mutex_lock(&iomutex);

    TraceFile  << "T" << id << " " << ip << ": W " << addr << endl;

    pthread_mutex_unlock(&iomutex);
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *)
{
    // instruments loads using a predicated call, i.e.
    // the call happens iff the load will be actually executed
    // (this does not matter for ia32 but arm and ipf have predicated instructions)
    if (INS_IsMemoryRead(ins))
    {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
            IARG_THREAD_ID,
            IARG_INST_PTR,
            IARG_MEMORYREAD_EA,
            IARG_END);
    }

    // instruments stores using a predicated call, i.e.
    // the call happens iff the store will be actually executed
    if (INS_IsMemoryWrite(ins))
    {
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
            IARG_THREAD_ID,
            IARG_INST_PTR,
            IARG_MEMORYWRITE_EA,
            IARG_END);
    }
}

    
/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    INS_AddInstrumentFunction(Instruction, 0);
    
    TraceFile << hex;
    
    PIN_StartProbedProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
