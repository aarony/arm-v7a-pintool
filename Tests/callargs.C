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
#include "pin.H"
#include <iostream>
#include <fstream>

#if defined(TARGET_MAC)
#define FUNC_PREFIX "_"
#else
#define FUNC_PREFIX
#endif

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

std::ofstream TraceFile;

/* ===================================================================== */

VOID MmapArgs(ADDRINT ra, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5, ADDRINT arg6)
{
    TraceFile << "mmap(" << arg1 << "," << arg2 << "," << arg3 << "," << arg4 << "," << arg5 << "," << arg6 << ")" << endl;
    TraceFile << "  called from " << ra << endl;
}

VOID CallArgs(ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5, ADDRINT arg6)
{
    TraceFile << "Call(" << arg1 << "," << arg2 << "," << arg3 << "," << arg4 << "," << arg5 << "," << arg6 << ")" << endl;
}

VOID FoobarArgs(ADDRINT arg1, ADDRINT arg2)
{
    TraceFile << "Foobar(" << arg1 << "," << arg2 << ")" << endl;
    if (arg1 != 0x0eadbeef || arg2 != 0x0eedfeed)
    {
        fprintf(stderr, "Error in arguments\n");
        abort();
    }
}

VOID BazArg(ADDRINT * arg1, ADDRINT * arg2, ADDRINT * arg3)
{
    TraceFile << "Baz(" << *arg1 << "," << *arg2 << "," << *arg3 << ")" << endl;
    *arg1 = 4;
    *arg2 = 5;
    *arg3 = 6;
}

LOCALVAR ADDRINT foobarAddress = 0;

/* ===================================================================== */

VOID Ins(INS ins, VOID *v)
{
    if (!INS_IsCall(ins))
        return;

    if (foobarAddress != 0
        && INS_IsDirectBranchOrCall(ins)
        && INS_DirectBranchOrCallTargetAddress(ins) == foobarAddress)
    {
        TraceFile << "Instrument call to foobar" << endl;
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(FoobarArgs), IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_END);
    }

    static BOOL first = true;

    if (!first)
        return;

    first = false;

    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CallArgs),  IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_G_ARG2_CALLER, IARG_G_ARG3_CALLER, IARG_G_ARG4_CALLER, IARG_G_ARG5_CALLER, IARG_END);
    
}

/* ===================================================================== */

VOID Image(IMG img, VOID *v)
{
    RTN mmapRtn = RTN_FindByName(img, FUNC_PREFIX "mmap");
    if (RTN_Valid(mmapRtn))
    {
        RTN_Open(mmapRtn);
        RTN_InsertCall(mmapRtn, IPOINT_BEFORE, AFUNPTR(MmapArgs), IARG_RETURN_IP, IARG_G_ARG0_CALLEE, IARG_G_ARG1_CALLEE, IARG_G_ARG2_CALLEE, IARG_G_ARG3_CALLEE, IARG_G_ARG4_CALLEE, IARG_G_ARG5_CALLEE, IARG_END);
        RTN_Close(mmapRtn);
    }
    RTN foobarRtn = RTN_FindByName(img, FUNC_PREFIX "foobar");
    if (RTN_Valid(foobarRtn))
    {
        foobarAddress = RTN_Address(foobarRtn);
        
        RTN_Open(foobarRtn);
        RTN_InsertCall(foobarRtn, IPOINT_BEFORE, AFUNPTR(FoobarArgs), IARG_G_ARG0_CALLEE, IARG_G_ARG1_CALLEE, IARG_END);
        RTN_Close(foobarRtn);
    }
#if !defined(TARGET_ARM)
    RTN bazRtn = RTN_FindByName(img, FUNC_PREFIX "baz");
    if (RTN_Valid(bazRtn))
    {
        RTN_Open(bazRtn);
        RTN_InsertCall(bazRtn, IPOINT_BEFORE, AFUNPTR(BazArg), IARG_FUNCARG_ENTRYPOINT_REFERENCE, 0, IARG_FUNCARG_ENTRYPOINT_REFERENCE, 1, IARG_FUNCARG_ENTRYPOINT_REFERENCE, 2, IARG_END);
        RTN_Close(bazRtn);
    }
#endif
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    TraceFile.close();
}

/* ===================================================================== */

int main(int argc, char *argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    TraceFile.open("args.out");

    TraceFile << hex;
    TraceFile.setf(ios::showbase);

    IMG_AddInstrumentFunction(Image, 0);
    INS_AddInstrumentFunction(Ins, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
