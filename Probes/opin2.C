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
/* ======================================================================= 
 * ORIGINAL AUTHOR: Vijay Janapa Reddi
 *
   ===================================================================== */
// This test places a probe on all control flow instructions, excluding
// the fall-though case.  It also calls two analysis functions, one before
// the probed instruction, and one after the probed instruction.  All this
// is done natively (not jitted).
//
// This test is not a general purpose tool.  It can only be used in specific
// situations.  Different compilers can cause different behavior.
// Problems have been seen when the compiler optimizes it register usage.
// In one case, the callee routine set ebx to the return address that was
// on the stack, and then the caller used this value in an address calculation.
// Once Pin was between the caller and the callee, a Pin address was on the top
// of the stack, ebx was set to an Pin address in the callee, and the address
// calculation in the caller was no longer valid.
//
// Also, placing breakpoints in the debugger when using probes must be done
// with care.  A break point too close to a probe site can cause unintended
// consequences.
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "pin.H"

using namespace std;

/* ===================================================================== */
VOID SetNativeXferForTraceExists(TRACE& trace)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS ins = BBL_InsTail(bbl);

        // These have to be explictly marked to go back to the original program
        if (INS_IsBranchOrCall(ins) || INS_IsRet(ins))
        {
            INS_SetNativeXfer(ins);
        }
    }
}

/* ===================================================================== */
INT32 _count = 0;
VOID SayHello(ADDRINT x)
{ 
    //cout << "Hello! " << hex << x << endl;
    _count++;
}

VOID SayBye(ADDRINT x)
{ 
    cout << "Bye! " << hex << x << endl; 
    cout << "_count " << _count << endl;
}

/* ===================================================================== */
VOID Image(IMG img, VOID * v)
{
    if (APP_ImgHead() != img)
        return;

    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        if (! SEC_IsExecutable(sec))
            continue;
        
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            RTN_Open(rtn);

            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                if (!INS_IsDirectBranchOrCall(ins))
                    continue;
                if (!INS_IsCall(ins))
                    continue;
                if (!INS_IsPatchable(ins))
                    continue;

                ADDRINT DstAddr = INS_DirectBranchOrCallTargetAddress(ins);
                ADDRINT SrcAddr = INS_Address(ins);
                
                // The compiler inserts a call to the following instruction so
                // as to push the IP onto the stack and pops it off immediately
                // in the next instruction so as to use the IP as an offset to
                // compute values for PIC code. 
                if (DstAddr == INS_NextAddress(ins))
                    continue;

                // Copy the callsite over into the trace
                TRACE Trace = TRACE_Allocate(SrcAddr);

                INS TraceIns = BBL_InsHead( TRACE_BblHead(Trace));

                INS_SetNativeXfer(TraceIns);
                INS_SetNativeCall(TraceIns);

                INS_InsertCall(TraceIns, IPOINT_BEFORE, (AFUNPTR) SayHello, IARG_INST_PTR, IARG_END);
                INS_InsertCall(TraceIns, IPOINT_AFTER, (AFUNPTR) SayBye, IARG_INST_PTR, IARG_END);
                //INS_InsertCall(TraceIns, IPOINT_TAKEN_BRANCH, (AFUNPTR) SayBye, IARG_INST_PTR, IARG_END);

                TRACE_GenerateCode(Trace);

                PIN_InsertProbe(SrcAddr, TRACE_CodeCacheAddress(Trace)); 

                TRACE_Deallocate(Trace);
            }

            RTN_Close(rtn);
        }
    }

    // Done creating traces for the native call targets so we 
    // request pin to let go of the application... sob sob :(
    PIN_Detach();
}

/* ===================================================================== */
int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    PIN_InitSymbols();

    IMG_AddInstrumentFunction(Image, 0);
 
    PIN_StartProgram(); 

    return 0;
}
