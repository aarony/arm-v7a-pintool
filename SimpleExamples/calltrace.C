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
  @ORIGINAL_AUTHOR: Robert Muth
*/

/* ===================================================================== */
/*! @file
 *  This file contains an ISA-portable PIN tool for tracing instructions
 */

#include "pin.H"
#include <iostream>
#include <fstream>

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

std::ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "calltrace.out", "specify trace file name");
KNOB<BOOL>   KnobPrintArgs(KNOB_MODE_WRITEONCE, "pintool", "a", "0", "print call arguments ");
//KNOB<BOOL>   KnobPrintArgs(KNOB_MODE_WRITEONCE, "pintool", "i", "0", "mark indirect calls ");

/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This tool produces a call trace.\n"
        "\n";

    
    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

string invalid = "invalid_rtn";

/* ===================================================================== */
const string *Target2String(ADDRINT target)
{
    string name = RTN_FindNameByAddress(target);
    if (name == "")
        return &invalid;
    else
        return new string(name);
}

/* ===================================================================== */

VOID  do_call_args(const string *s, ADDRINT arg0)
{
    TraceFile << *s << "(" << arg0 << ",...)" << endl;
}

/* ===================================================================== */

VOID  do_call_args_indirect(ADDRINT target, BOOL taken, ADDRINT arg0)
{
    if( !taken ) return;
    
    const string *s = Target2String(target);
    do_call_args(s, arg0);

    if (s != &invalid)
        delete s;
}

/* ===================================================================== */

VOID  do_call(const string *s)
{
    TraceFile << *s << endl;
}

/* ===================================================================== */

VOID  do_call_indirect(ADDRINT target, BOOL taken)
{
    if( !taken ) return;

    const string *s = Target2String(target);
    do_call( s );
    
    if (s != &invalid)
        delete s;
}

/* ===================================================================== */

VOID Trace(TRACE trace, VOID *v)
{
    const BOOL print_args = KnobPrintArgs.Value();
    
        
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS tail = BBL_InsTail(bbl);
        
        if( INS_IsCall(tail) )
        {
            if( INS_IsDirectBranchOrCall(tail) )
            {
                const ADDRINT target = INS_DirectBranchOrCallTargetAddress(tail);
                if( print_args )
                {
                    INS_InsertPredicatedCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_args),
                                             IARG_PTR, Target2String(target), IARG_G_ARG0_CALLER, IARG_END);
                }
                else
                {
                    INS_InsertPredicatedCall(tail, IPOINT_BEFORE, AFUNPTR(do_call),
                                             IARG_PTR, Target2String(target), IARG_END);
                }
                
            }
            else
            {
                if( print_args )
                {
                    INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_args_indirect),
                                   IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN,  IARG_G_ARG0_CALLER, IARG_END);
                }
                else
                {
                    INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                                   IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_END);
                }
                
                
            }
        }
        else
        {
            // On em64t, some of the code is not in an image
            RTN rtn = TRACE_Rtn(trace);
            
            // also track stup jumps into share libraries
            if( RTN_Valid(rtn) && !INS_IsDirectBranchOrCall(tail) && ".plt" == SEC_Name( RTN_Sec( rtn ) ))
            {
                if( print_args )
                {
                    INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_args_indirect),
                                   IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN,  IARG_G_ARG0_CALLER, IARG_END);
                }
                else
                {
                    INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                                   IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_END);

                }
            }
        }
        
    }
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    TraceFile << "# eof" << endl;
    
    TraceFile.close();
}

/* ===================================================================== */

int  main(int argc, char *argv[])
{
    
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    

    TraceFile.open(KnobOutputFile.Value().c_str());

    TraceFile << hex;
    TraceFile.setf(ios::showbase);
    
    string trace_header = string("#\n"
                                 "# Call Trace Generated By Pin\n"
                                 "#\n");
    

    TraceFile.write(trace_header.c_str(),trace_header.size());
    
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
