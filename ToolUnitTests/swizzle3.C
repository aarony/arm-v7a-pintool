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
#include <assert.h>
#include <stdio.h>
#include <set>
#include "pin.H"

UINT64 icount = 0;

const ADDRINT TargetPrefix = 0xb0000000;
const ADDRINT SwizzlePrefix = 0xb1000000;

LOCALFUN ADDRINT Prefix(ADDRINT val)
{
    return val & 0xff000000;
}

LOCALFUN BOOL TargetSpace(ADDRINT val)
{
    return Prefix(val) == TargetPrefix;
}

LOCALFUN BOOL SwizzleSpace(ADDRINT val)
{
    return Prefix(val) == SwizzlePrefix;
}

LOCALFUN BOOL SwizzleSpace1(ADDRINT val)
{
    return SwizzleSpace(val);
}

LOCALFUN BOOL SwizzleSpace2(ADDRINT val1, ADDRINT val2)
{
    return SwizzleSpace(val1) || SwizzleSpace(val2);
}

LOCALFUN ADDRINT Unswizzle(ADDRINT val)
{
    fprintf(stderr, "Unswizzling %p\n", (void*)val);

    assert(SwizzleSpace(val));

    return (val & ~ SwizzlePrefix) | TargetPrefix;
}

LOCALFUN ADDRINT Swizzle(ADDRINT val)
{
    fprintf(stderr, "Swizzling %p\n", (void*)val);

    assert(TargetSpace(val));

    return (val & ~ TargetPrefix) | SwizzlePrefix;
}

    

ADDRINT ProcessAddress(ADDRINT val, VOID *ip)
{
    if (TargetSpace(val))
        fprintf(stderr, "Unexpected reference to target space: %p at ip %p\n", (void*)val, ip);
    
    if (SwizzleSpace(val))
    {
        //fprintf(stderr, "Unswizzle ip %p\n", ip);
        return Unswizzle(val);
    }

    return val;
}

VOID SwizzleArg(ADDRINT * arg)
{
    ASSERTX(SwizzleSpace(*arg));
    *arg = ProcessAddress(*arg, 0);
}

// When an image is loaded, check for a MyAlloc function
VOID Image(IMG img, VOID *v)
{
    //fprintf(stderr, "Loading %s\n",IMG_name(img));
    
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        //fprintf(stderr, "  sec %s\n", SEC_name(sec).c_str());
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            //fprintf(stderr, "    rtn %s\n", RTN_name(rtn).c_str());
            // Swizzle the return value of MyAlloc
            
#if defined(TARGET_MAC)
            if (RTN_Name(rtn) == "_MyAlloc")
#else            
            if (RTN_Name(rtn) == "MyAlloc")
#endif 
            {
                RTN_Open(rtn);
                
                fprintf(stderr, "Adding swizzle to %s\n", "MyAlloc");
                RTN_InsertCall(rtn, IPOINT_AFTER, AFUNPTR(Swizzle), IARG_REG_VALUE, REG_GAX, IARG_RETURN_REGS, REG_GAX, IARG_END);
                RTN_Close(rtn);
            }

#if defined(TARGET_MAC)            
            if (RTN_Name(rtn) == "_MyFree")
#else
            if (RTN_Name(rtn) == "MyFree")    
#endif 
            {
                RTN_Open(rtn);

                fprintf(stderr, "Adding unswizzle to %s\n", "MyFree");
                RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(SwizzleArg), IARG_FUNCARG_ENTRYPOINT_REFERENCE, 0, IARG_END);
                RTN_Close(rtn);
            }
        }
    }
}

VOID RewriteIns(INS ins)
{
    //fprintf(stderr,"Rewriting %p\n",(void*)INS_Address(ins));
    
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ, REG_INST_G0))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G0, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_WRITE, REG_INST_G1))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYWRITE_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G1, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ2, REG_INST_G2))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD2_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G2, IARG_END);
    }
}

set<ADDRINT> SwizzleRefs;

VOID Restart(ADDRINT insAddress, CONTEXT * ctxt, ADDRINT traceAddress)
{
    // This instruction requires swizzling
    SwizzleRefs.insert(insAddress);
    
    //fprintf(stderr,"Remove trace at address %p\n",(void*)traceAddress);
    CODECACHE_InvalidateTraceAtProgramAddress(traceAddress);

    //fprintf(stderr,"Restart %p\n",(void*)insAddress);
    PIN_ExecuteAt(ctxt);
}

// Generate code to check if swizzling is needed, but not do it
VOID CheckIns(INS ins, ADDRINT traceAddress)
{
    if (INS_IsMemoryRead(ins) && INS_HasMemoryRead2(ins))
    {
        INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(SwizzleSpace2), IARG_MEMORYREAD_EA, IARG_MEMORYREAD2_EA, IARG_END);
    }
    else if (INS_IsMemoryRead(ins) && INS_IsMemoryWrite(ins))
    {
        INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(SwizzleSpace2), IARG_MEMORYREAD_EA, IARG_MEMORYWRITE_EA, IARG_END);
    }
    else if (INS_IsMemoryRead(ins))
    {
        INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(SwizzleSpace1), IARG_MEMORYREAD_EA, IARG_END);
    }
    else if (INS_IsMemoryWrite(ins))
    {
        INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(SwizzleSpace1), IARG_MEMORYWRITE_EA, IARG_END);
    }
    else
    {
        return;
    }
    
    INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(Restart), IARG_INST_PTR, IARG_CONTEXT, IARG_ADDRINT, traceAddress, IARG_END);
}
    
VOID Trace(TRACE trace, VOID *v)
{
    BOOL rewrite = false;
    
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            // If we see an instruction that needs rewriting, then rewrite all
            if (SwizzleRefs.find(INS_Address(ins)) != SwizzleRefs.end())
                rewrite = true;
        
            if (rewrite)
            {
                // If we suspect this instruction needs to be swizzled, generate safe, but slow code
                RewriteIns(ins);
            }
            else
            {
                // Generate code to check if swizzling is needed, but not do it
                CheckIns(ins, TRACE_Address(trace));
            }
        }
    }
}

int main(int argc, char * argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    TRACE_AddInstrumentFunction(Trace, 0);
    IMG_AddInstrumentFunction(Image, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
