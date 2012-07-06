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
//  ORIGINAL_AUTHOR: Gail Lyons
//
//  This tool tests the functionality if CODECACHE::GetOriginalAddress();
//

#include "pin.H"
#include "utils.H"
#include "portability.H"
#include <iostream>
#include <fstream>



using namespace std;

/* ================================================================== */
/* Global Variables                                                   */
/* ================================================================== */
ofstream TraceFile;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "orig_address.out", "specify trace file name");

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

/* ================================================================== */
/*
 This routine is called once at the start of the cache.
*/
VOID OpenFile()
{
    string logFileName = KnobOutputFile.Value();
    TraceFile.open(logFileName.c_str());
}

/* ================================================================== */
/*
 Print details of traces as they are inserted
*/
VOID Trace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            if ( INS_IsMemoryRead(ins) ) 
            {
                ADDRINT cache_pc = INS_CodeCacheAddress(ins);

                if ( cache_pc != 0x0 )
                {
                    ADDRINT orig_pc = CODECACHE_OriginalAddress(cache_pc);

                    TraceFile << "Code cache: ";
                    TraceFile << "\tcache pc=0x" << hex << cache_pc;
                    TraceFile << "\torig pc=0x" << hex << orig_pc;

                    if ( INS_IsOriginal(ins) ) 
                    {
                        if ( INS_Address(ins) == orig_pc )
                            TraceFile << "\tSuccess." << endl;
                        else 
                        {
                            TraceFile << "\tFailure." << endl;
                            TraceFile << IMG_Name( SEC_Img (RTN_Sec (INS_Rtn (ins) )));
                            TraceFile << ": \tINS_Address(ins)=0x" << hex << INS_Address(ins);
                            TraceFile << "\tCODECACHE_GetOriginalAddress=0x" << hex << orig_pc << endl;
                        }
                    }
                    else
                        TraceFile << "\t\tNot original." << endl;
                }
            }
        }
    }
}


/* ================================================================== */
/*
 This routine is called once at the end.
*/
VOID CloseFile(INT32 c, VOID *v)
{
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

    // Register a routine that gets called at the start
    CODECACHE_AddCacheInitFunction(OpenFile, 0);

    // Register a routine that gets called when a trace is
    //  inserted into the codecache
    CODECACHE_AddTraceInsertedFunction(Trace, 0);
    
    // Register a routine that gets called when the program ends
    PIN_AddFiniFunction(CloseFile, 0);
    
    PIN_StartProgram();  // Never returns
    
    return 0;
}
