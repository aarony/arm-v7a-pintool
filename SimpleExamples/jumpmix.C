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
#include "portability.H"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,         "pintool",
                            "o", "jumpmix.out", "specify profile file name");

KNOB<BOOL>   KnobPid(KNOB_MODE_WRITEONCE,                "pintool",
                     "i", "0", "append pid to output");


/* ===================================================================== */

static INT32 Usage()
{
    cerr << "This pin tool collects a profile of jump/branch/call instructions for an application\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;
    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */



class COUNTER
{
  public:
    UINT64 _call;
    UINT64 _call_indirect;
    UINT64 _return;
    UINT64 _syscall;
    UINT64 _branch;
    UINT64 _branch_indirect;    

    COUNTER() : _call(0),_call_indirect(0), _return(0), _branch(0), _branch_indirect(0)   {}

    UINT64 Total()
    {
        return _call + _call_indirect + _return + _syscall + _branch + _branch_indirect;
    }
};

COUNTER CountSeen;
COUNTER CountTaken;


#define INC(what) VOID inc ## what (INT32 taken) { CountSeen. what ++; if( taken) CountTaken. what ++;}

INC(_call)
INC(_call_indirect)
INC(_branch)
INC(_branch_indirect)
INC(_syscall)
INC(_return)

    


/* ===================================================================== */

VOID Instruction(INS ins, void *v)
{
    if( INS_IsRet(ins) )
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_return, IARG_BRANCH_TAKEN,  IARG_END);
    }
    else if( INS_IsSyscall(ins) )
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_syscall, IARG_BRANCH_TAKEN,  IARG_END);
    }
    else if (INS_IsDirectBranchOrCall(ins))
    {
        if( INS_IsCall(ins) )
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_call, IARG_BRANCH_TAKEN,  IARG_END);
        else
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_branch, IARG_BRANCH_TAKEN,  IARG_END);
    }
    else if( INS_IsIndirectBranchOrCall(ins) )
    {
        if( INS_IsCall(ins) )
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_call_indirect, IARG_BRANCH_TAKEN,  IARG_END);
        else
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) inc_branch_indirect, IARG_BRANCH_TAKEN,  IARG_END);
    }

}

/* ===================================================================== */

#define OUT(n, a, b) *out << n << " " << a << setw(16) << CountSeen. b  << " " << setw(16) << CountTaken. b << endl

static std::ofstream* out = 0;

VOID Fini(int n, void *v)
{
    SetAddress0x(1);


    *out << "# JUMPMIX\n";
    *out << "#\n";
    *out << "# $dynamic-counts\n";
    *out << "#\n";
    

    
    *out << "4000 *total "  << setw(16) << CountSeen.Total() << " " << setw(16) << CountTaken.Total() << endl;
    
        
    OUT(4010, "call            ",_call);
    OUT(4011, "indirect-call   ",_call_indirect);
    OUT(4012, "branch          ",_branch);
    OUT(4013, "indirect-branch ",_branch_indirect);
    OUT(4014, "syscall         ",_syscall);
    OUT(4015, "return          ",_return);

    *out << "#\n";
    *out << "# eof\n";
    out->close();
    
}



/* ===================================================================== */

int main(int argc, char *argv[])
{
    
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    string filename =  KnobOutputFile.Value();
    if( KnobPid )        filename += "." + decstr( getpid_portable() );
    out = new std::ofstream(filename.c_str());

        
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
