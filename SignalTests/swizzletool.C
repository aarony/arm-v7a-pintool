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
/*
 * See "swizzleapp.c" for a description of this test.
 */

#include <signal.h>
#include <iostream>
#include <set>
#include "pin.H"

#define MASK 0xC0000000
#define TAG  0xC0000000


static VOID InstrumentImage(IMG, VOID *);
static VOID Swizzle(ADDRINT *);
static VOID InstrumentTrace(TRACE, VOID *);
static VOID RewriteIns(INS);
static ADDRINT Unswizzle(ADDRINT);
static BOOL SegvHandler(INT32, CONTEXT *, BOOL hndlr, void *);


set<ADDRINT> SwizzleRefs;

int main(int argc, char * argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    IMG_AddInstrumentFunction(InstrumentImage, 0);
    TRACE_AddInstrumentFunction(InstrumentTrace, 0);
    PIN_AddSignalInterceptFunction(SIGSEGV, SegvHandler, 0);

    // Never returns
    PIN_StartProgram();
    return 0;
}


static VOID InstrumentImage(IMG img, VOID *v)
{
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            if (RTN_Name(rtn) == "Allocate")
            {
                RTN_Open(rtn);
                RTN_InsertCall(rtn, IPOINT_AFTER, AFUNPTR(Swizzle),
                    IARG_FUNCRET_EXITPOINT_REFERENCE, IARG_END);
                RTN_Close(rtn);
            }
        }
    }
}


static VOID Swizzle(ADDRINT *val)
{
    if ((*val & MASK) != 0)
        cerr << "Invalid test" << endl;
    *val |= TAG;
}


static VOID InstrumentTrace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            // If we see an instruction that needs rewriting, then rewrite all
            if (SwizzleRefs.find(INS_Address(ins)) != SwizzleRefs.end())
            {
                // If we suspect this instruction needs to be swizzled, generate safe, but slow code
                RewriteIns(ins);
            }
        }
    }
}


static VOID RewriteIns(INS ins)
{
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ, REG_INST_G0))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYREAD_EA, IARG_RETURN_REGS, REG_INST_G0, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_WRITE, REG_INST_G1))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYWRITE_EA, IARG_RETURN_REGS, REG_INST_G1, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ2, REG_INST_G2))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYREAD2_EA, IARG_RETURN_REGS, REG_INST_G2, IARG_END);
    }
}


static ADDRINT Unswizzle(ADDRINT val)
{
    if ((val & MASK) == TAG)
        return val & ~MASK;
    return val;
}


static BOOL SegvHandler(INT32 sig, CONTEXT *ctxt, BOOL hndlr, void *v)
{
    ADDRINT address = PIN_GetContextReg(ctxt, REG_INST_PTR);

    if (SwizzleRefs.find(address) != SwizzleRefs.end())
    {
        cerr << "Multiple faults at same instruction" << endl;
        exit(1);
    }

    SwizzleRefs.insert(address);
    CODECACHE_InvalidateRange(address, address);

    return false;
}
