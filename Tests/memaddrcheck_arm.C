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
#include <stdio.h>
#include "pin.H"
#include "pin_isa.H"

FILE * trace;

UINT64 mcount = 0;
UINT64 error = 0;

VOID * pc;
UINT32 * ea;
UINT32 mem_value_before;
UINT32 reg_value_before;

struct memop_info
{
    UINT32 num_bytes  : 3;
    UINT32 does_load  : 1;
    UINT32 does_store : 1;
    UINT32 is_signed  : 1;
};

VOID CountError()
{
    error++;
    if (error > 100)
    {
        fprintf(stderr, "Too many errors, giving up\n");
        exit(error);
    }
}

VOID RecordMem(VOID * iaddr, UINT32 * memaddr, UINT32 regval)
{
    pc = iaddr;
    ea = memaddr;
    mem_value_before = *ea;
    reg_value_before = regval;
}

VOID CheckAny(UINT32 reg_value, UINT32 mem_value)
{
    if (reg_value != mem_value)
    {
        fprintf(stderr, "error: PC %p, mem addr%p, register value 0x%08x != memory value 0x%08x\n",
            pc, ea, reg_value, mem_value);
        CountError();
    }

    mcount++;
    if ((mcount % 1000) == 0)
    {
        fprintf(stderr, "%lld memops checked\n", mcount);
    }
}

// check load value
VOID CheckLoad(struct memop_info memop, UINT32 reg_value_after)
{
    const UINT32 mask = (1 << memop.num_bytes * 8) - 1;
    UINT32 mem_value = mem_value_before;
    UINT32 reg_value = reg_value_after;

    mem_value &= mask;

    if (memop.is_signed)
    {
        const UINT32 shift = 32 - (memop.num_bytes * 8);
        INT32 mem_value_signed;
        mem_value_signed = mem_value << shift;
        mem_value = mem_value_signed >> shift;
    }

    CheckAny(reg_value, mem_value);
}

// check store value
VOID CheckStore(struct memop_info memop)
{
    const UINT32 mem_value_after = *ea;
    const UINT32 mask = (1 << memop.num_bytes * 8) - 1;
    UINT32 mem_value = mem_value_after;
    UINT32 reg_value = reg_value_before;

    mem_value &= mask;
    reg_value &= mask;

    CheckAny(reg_value, mem_value);
}

// check memory values against register values
VOID CheckMem(struct memop_info memop, UINT32 reg_value_after)
{
    if ( memop.does_load)  CheckLoad(memop, reg_value_after);
    if ( memop.does_store) CheckStore(memop);
}

REG FirstRegInRegList(INS ins)
{
    //NB: there are predefined functions for this in the API
    // we just doing this here the hard way for instructional purposes
    
    // hand-decode the instruction and pull out the register list
    const UINT32 * iaddr = (UINT32 *) INS_Address(ins);
    const UINT32 bits = *iaddr;
    const UINT32 reg_list = bits & 0xffff;

    int i;
    for (i = 0; i < 16; i++)
    {
        if (reg_list & (1 << i)) return REG(REG_R0 + i);
    }

    return REG_INVALID_;
}

VOID Instruction(INS ins, VOID *v)
{
    if ( ! INS_IsMemoryRead(ins) && ! INS_IsMemoryWrite(ins)) return;
    if ( ! INS_HasFallThrough(ins)) return; // IPOINT_AFTER illegal here

    const OPCODE op = INS_Opcode(ins);

    // describe memop properties
    struct memop_info memop = {0};
    memop.does_load  = INS_IsMemoryRead(ins);
    memop.does_store = INS_IsMemoryWrite(ins);
    // on ARM, read and write sizes are the same
    const UINT32 size = INS_MemoryReadSize(ins);
    // we only check up to the first word
    memop.num_bytes = (size <= 4) ? size : 4;
    
    REG reg_store = REG_INVALID_;
    REG reg_load = REG_INVALID_;
    switch (op)
    {
        // regular memops
      case ARM_OPCODE_LDRS_1:
      case ARM_OPCODE_LDRS_2:
        memop.is_signed  = 1;
        // intentional fall-through
      case ARM_OPCODE_LDR_1:
      case ARM_OPCODE_LDR_2:
      case ARM_OPCODE_LDR_4:
        reg_load = INS_RegnameRd(ins);
        break;
        
      case ARM_OPCODE_STR_1:
      case ARM_OPCODE_STR_2:
      case ARM_OPCODE_STR_4:
        reg_store = INS_RegnameRd(ins);
        break;

        // special memops
      case ARM_OPCODE_SWP_1:
      case ARM_OPCODE_SWP_4:
        reg_store = INS_RegnameRm(ins);
        reg_load = INS_RegnameRd(ins);
        break;
        
          // multi-memops
      case ARM_OPCODE_LDM:
      case ARM_OPCODE_LDM_U:
        reg_load = FirstRegInRegList(ins); 
        break;
        
      case ARM_OPCODE_STM:
      case ARM_OPCODE_STM_U:
        reg_store = FirstRegInRegList(ins);
        break;
    }
    
    // add instrumentation
    switch (op)
    {
        // regular memops
      case ARM_OPCODE_LDRS_1:
      case ARM_OPCODE_LDR_1:
      case ARM_OPCODE_LDRS_2:
      case ARM_OPCODE_LDR_2:
      case ARM_OPCODE_LDR_4:
      case ARM_OPCODE_STR_1:
      case ARM_OPCODE_STR_2:
      case ARM_OPCODE_STR_4:
        // special memops
      case ARM_OPCODE_SWP_1:
      case ARM_OPCODE_SWP_4:
        // multi-memops
      case ARM_OPCODE_LDM:
      case ARM_OPCODE_LDM_U:
      case ARM_OPCODE_STM:
      case ARM_OPCODE_STM_U:
        // PC & EA recording before memop
        if (reg_store != REG_INVALID_)
        {
            INS_InsertPredicatedCall(ins,
                                     IPOINT_BEFORE, AFUNPTR(RecordMem),
                                     IARG_INST_PTR,
                                     IARG_MEMORYWRITE_EA,
                                     IARG_REG_VALUE, reg_store,
                                     IARG_END);
        }
        else
        {
            INS_InsertPredicatedCall( ins,
                                      IPOINT_BEFORE, AFUNPTR(RecordMem),
                                      IARG_INST_PTR,
                                      IARG_MEMORYREAD_EA,
                                      IARG_UINT32, 0,
                                      IARG_END);
        }

        // check after memop
        if (reg_load != REG_INVALID_)
        {
            INS_InsertPredicatedCall( ins,
                                      IPOINT_AFTER, AFUNPTR(CheckMem),
                                      IARG_UINT32, memop,
                                      IARG_REG_VALUE, reg_load,
                                      IARG_END);
        }
        else
        {
            INS_InsertPredicatedCall( ins,
                                      IPOINT_AFTER, AFUNPTR(CheckMem),
                                      IARG_UINT32, memop,
                                      IARG_UINT32, 0,
                                      IARG_END);
        }
        break;

        // copro/FP memops
      case ARM_OPCODE_LDFS:
      case ARM_OPCODE_LDFD:
      case ARM_OPCODE_LDFE:
      case ARM_OPCODE_LDFP:
      case ARM_OPCODE_LFM:
      case ARM_OPCODE_STFS:
      case ARM_OPCODE_STFD:
      case ARM_OPCODE_STFE:
      case ARM_OPCODE_STFP:
      case ARM_OPCODE_SFM:
        // we don't handle FP memops
        break;
    }
    
}

VOID Fini(int code, void *v)
{
    fprintf(stderr, "%lld errors (%lld memops checked)\n", error, mcount);
    exit(error);
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
