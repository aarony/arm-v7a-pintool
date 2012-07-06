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

#define READSHADOW REG_INST_G8
#define WRITESHADOW REG_INST_G9
#define READ2SHADOW REG_INST_G3

BOOL GpReg(REG reg)
{
    if (reg == REG_ESP)
        return false;

    return reg >= REG_EDI && reg <= REG_EAX;
}

INT32 RegIndex(REG reg)
{
    ASSERTX(GpReg(reg));
    
    return reg - REG_EDI;
}

REG IndexToReg(INT32 index)
{
    return REG(index + REG_EDI);
}

REG ShadowReg(REG reg)
{
    REG shadow = REG(RegIndex(reg) + REG_INST_G0);
    switch(shadow)
    {
      case READSHADOW:
      case READ2SHADOW:
      case WRITESHADOW:
        ASSERTX(false);
        break;
      default:
        break;
    }

    return shadow;
}

#define REGCOUNT (REG_EAX - REG_EDI + 1)


VOID CheckEffectiveAddress(INS ins)
{
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ, READSHADOW))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, READSHADOW, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_WRITE, WRITESHADOW))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYWRITE_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, WRITESHADOW, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ2, READ2SHADOW))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD2_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, READ2SHADOW, IARG_END);
    }
}


VOID WriteShadows(INS ins, BOOL * live)
{
    for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
    {
        REG reg = INS_RegW(ins, i);

        if (!GpReg(reg))
            continue;
        
        if (!live[RegIndex(reg)])
            continue;
        
        // If this instruction writes a register that is used as a base register in
        // a memory operation later in the trace, then translate the address and
        // write it to a shadow register
        INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(ProcessAddress), IARG_REG_VALUE, reg, IARG_INST_PTR, IARG_RETURN_REGS, ShadowReg(reg), IARG_END);
        live[RegIndex(reg)] = false;
    }
}
        
        
VOID RewriteBases(INS ins, BOOL * live)
{
    for (UINT32 i = 0; i < INS_OperandCount(ins); i++)
    {
        if (!INS_OperandIsMemory(ins, i))
            continue;
        
        if (INS_OperandMemoryIndexReg(ins, i) != REG_INVALID())
        {
            CheckEffectiveAddress(ins);
            return;
        }
            
        REG baseReg = INS_OperandMemoryBaseReg(ins, i);

        // If no basereg is used, then it must be an absolute address
        if (baseReg == REG_INVALID())
            continue;

        // No need to rewrite stack references
        if (baseReg == REG_ESP)
            continue;
        
        // If we reach this point, we have an instruction that
        // must be rewritten, but if the memory operand is
        // implicit, we can't rewrite the base register
        if (INS_OperandIsImplicit(ins, i))
        {
            CheckEffectiveAddress(ins);
            return;
        }
        
        REG shadowReg = ShadowReg(baseReg);
        INS_OperandMemorySetBaseReg(ins, i, shadowReg);

        // Remember to write the shadow register
        live[RegIndex(baseReg)] = true;
    }
}

    
// If a base register is used in the trace but not written
// in the trace, then write the shadow register at the top
// of the trace
VOID WriteLiveShadows(TRACE trace, BOOL * live)
{
    for (INT32 i = 0; i < REGCOUNT; i++)
    {
        if (live[i])
        {
            REG reg = IndexToReg(i);
            
            // If this instruction writes a register that is used as a base register in
            // a memory operation later in the trace, then translate the address and
            // write it to a shadow register
            TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(ProcessAddress), IARG_REG_VALUE, reg, IARG_INST_PTR, IARG_RETURN_REGS, ShadowReg(reg), IARG_END);
        }
    }
}

    
VOID DumpTrace(CHAR * message, TRACE trace)
{
    fprintf(stderr,"\n%s:\n",message);
    
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            fprintf(stderr,"%p %s\n",(void*)INS_Address(ins),INS_Disassemble(ins).c_str());
        }
    }
}


VOID Trace(TRACE trace, VOID *v)
{
    //DumpTrace("Before", trace);
    
    BOOL live[REGCOUNT];
    for (INT32 i = 0; i < REGCOUNT; i++)
    {
        live[i] = false;
    }
    
    for (BBL bbl = TRACE_BblTail(trace); BBL_Valid(bbl); bbl = BBL_Prev(bbl))
    {
        for (INS ins = BBL_InsTail(bbl); INS_Valid(ins); ins = INS_Prev(ins))
        {
            WriteShadows(ins, live);
            
            RewriteBases(ins, live);
        }
    }

    WriteLiveShadows(trace, live);
    //DumpTrace("After", trace);
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
