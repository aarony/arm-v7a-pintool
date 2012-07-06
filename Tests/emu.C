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
#include <iostream>

UINT32 icount = 0;

LOCALVAR KNOB<UINT32> KnobCount(KNOB_MODE_WRITEONCE, "pintool", "count", "100000000",
                                                  "Explicitly save and restore the flags in inlined analysis routines");
VOID Emulate2Address(OPCODE opcode,
                     INS ins,
                     ADDRINT (*OpRM)(ADDRINT,ADDRINT*),
                     ADDRINT (*OpRV)(ADDRINT,ADDRINT),
                     VOID (*OpMV)(ADDRINT*,ADDRINT))
{
    if (INS_Opcode(ins) != opcode)
        return;
    
    if (INS_OperandIsMemory(ins, 0)
        // This will filter out segment overrides
        && INS_IsMemoryWrite(ins))
    {
        if (INS_OperandIsReg(ins, 1)
            && REG_is_gr(INS_OperandReg(ins, 1)))
        {
            // Source register, dst memory
            INS_InsertCall(ins,
                           IPOINT_BEFORE,
                           AFUNPTR(OpMV),
                           IARG_MEMORYWRITE_EA,
                           IARG_REG_VALUE,
                           INS_OperandReg(ins, 1),
                           IARG_END);
            INS_Delete(ins);
        }
        else
        {
            ASSERTX(!INS_OperandIsMemory(ins, 1));
        }
    }
    else if (INS_OperandIsReg(ins, 0))
    {
        ASSERTX(INS_OperandIsReg(ins, 0));

        if (INS_OperandIsReg(ins, 1))
        {
            // Source register, dst register
            INS_InsertCall(ins,
                           IPOINT_BEFORE,
                           AFUNPTR(OpRV),
                           IARG_REG_VALUE,
                           INS_OperandReg(ins, 0),
                           IARG_REG_VALUE,
                           INS_OperandReg(ins, 1),
                           IARG_RETURN_REGS,
                           INS_OperandReg(ins, 0),
                           IARG_END);
            INS_Delete(ins);
        }
        else if (INS_OperandIsMemory(ins, 1)
                 // This will filter out segment overrides
                 && INS_IsMemoryRead(ins))
        {
            // Source register, dst register
            INS_InsertCall(ins,
                           IPOINT_BEFORE,
                           AFUNPTR(OpRM),
                           IARG_REG_VALUE,
                           INS_OperandReg(ins, 0),
                           IARG_MEMORYREAD_EA,
                           IARG_RETURN_REGS,
                           INS_OperandReg(ins, 0),
                           IARG_END);
            INS_Delete(ins);
        }
    }
    
#if 0
    if (KnobCount == icount)
        fprintf(stderr,"Last one %s\n",INS_Disassemble(ins).c_str());
    else if (icount > KnobCount)
        return;
    icount++;
#endif

}

// Move a register or literal to memory
VOID MovMV(ADDRINT * op0, ADDRINT op1)
{ 
    *op0 = op1;
}
    
// Move a literal or register to a register
ADDRINT MovRV(ADDRINT op0, ADDRINT op1)
{ 
    return op1;
}
    
// Move from memory to register
ADDRINT MovRM(ADDRINT op0, ADDRINT * op1)
{ 
    return *op1;
}
    
VOID Instruction(INS ins, VOID *v)
{
    Emulate2Address(XEDICLASS_MOV, ins, MovRM, MovRV, MovMV);
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
