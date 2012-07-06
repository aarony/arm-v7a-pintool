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
#include <iostream>
#include "pin.H"


#ifndef ASM
extern "C" VOID* StackUnswitch(AFUNPTR fun, VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4, ADDRINT appSp);
#endif
    
ADDRINT foo(ADDRINT x1, ADDRINT x2, ADDRINT x3, ADDRINT x4)
{
    static BOOL first = TRUE;

    if (first)
    {
        first = FALSE;
        //cout << "In foo(): x1="<<decstr(x1)<<" x2="<<decstr(x2)<<" x3="<<decstr(x3)<<" x4="<<decstr(x4)<<endl;
    }
    
    return x1+x2+x3+x4;
}

ADDRINT bar(ADDRINT x1, ADDRINT x2)
{
    static BOOL first = TRUE;

    if (first)
    {
        first = FALSE;
        //cout << "In bar(): x1="<<decstr(x1)<<" x2="<<decstr(x2)<<endl;
    }
    
    return x1*x2;
}


GLOBALCFUN VOID* MyDispatch(AFUNPTR fun, VOID* arg1, VOID* arg2, VOID* arg3, VOID* arg4)
{
    if (fun == reinterpret_cast<AFUNPTR>(foo))
    {  // call foo()
        return reinterpret_cast<VOID*>(foo(reinterpret_cast<ADDRINT>(arg1),
                                           reinterpret_cast<ADDRINT>(arg2),
                                           reinterpret_cast<ADDRINT>(arg3),
                                           reinterpret_cast<ADDRINT>(arg4)));
    }
    else if (fun == reinterpret_cast<AFUNPTR>(bar))
    {  // call bar()
        return reinterpret_cast<VOID*>(bar(reinterpret_cast<ADDRINT>(arg1),
                                           reinterpret_cast<ADDRINT>(arg2)));
    } 
    ASSERT(false,"MyDistpatch(): unknown function\n");
    
    return 0;
}

VOID DoBbl(INT32 threadId)
{

    const ADDRINT appSp = PIN_FindAlternateAppStack();

    if (threadId == 1)
    {
        reinterpret_cast<ADDRINT>(StackUnswitch(reinterpret_cast<AFUNPTR>(foo),
                                                reinterpret_cast<VOID*>(10), reinterpret_cast<VOID*>(20),
                                                reinterpret_cast<VOID*>(30), reinterpret_cast<VOID*>(40),
                                                appSp));
    }
    else if (threadId == 2)
    {
        reinterpret_cast<ADDRINT>(StackUnswitch(reinterpret_cast<AFUNPTR>(bar),
                                                reinterpret_cast<VOID*>(100), reinterpret_cast<VOID*>(200),
                                                reinterpret_cast<VOID*>(0), reinterpret_cast<VOID*>(0),
                                                appSp));
    }
    
        
    //cout << "Result of calling foo = "<<decstr(result)<<endl;
    
}

// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
         BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)DoBbl, IARG_THREAD_ID, IARG_END);
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{
    // Initialize pin
    PIN_Init(argc, argv);

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
