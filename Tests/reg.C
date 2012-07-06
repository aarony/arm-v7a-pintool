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
#include <fstream>
#include "pin.H"

ofstream out("reg.out");

UINT64 icount = 0;

VOID docount(VOID * ip, VOID * reg)
{
    icount++;
    if ((icount % 1000) == 1)
        out << "ip:" << ip << " count:" << icount << " SP:" << reg << endl;
}
    
VOID Instruction(INS ins, VOID *v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount,
                   IARG_INST_PTR, IARG_REG_VALUE, REG_STACK_PTR, IARG_END);
}

VOID PrintError(ADDRINT ip, ADDRINT al, ADDRINT ah, ADDRINT ax, ADDRINT eax, ADDRINT rax)
{
    std::cerr << hex << "ip " << ip
              << " al " << al
              << " ah " << ah
              << " ax " << ax
              << " eax " << eax
              << " rax " << rax
              << endl;
    exit(1);
}
    
VOID CheckRegs(ADDRINT ip, ADDRINT al, ADDRINT ah, ADDRINT ax, ADDRINT eax, ADDRINT rax)
{
    if ((eax & 0xff) != al)
    {
        PrintError(ip, al, ah, ax, eax, rax);
    }
    
    if (((eax & 0xff00) >> 8) != ah)
    {
        PrintError(ip, al, ah, ax, eax, rax);
    }

    if ((eax & 0xffff) != ax)
    {
        PrintError(ip, al, ah, ax, eax, rax);
    }

#if defined(TARGET_IA32E)
    if((rax & 0xffffffff) != eax)
    {
        PrintError(ip, al, ah, ax, eax, rax);
    }
#endif
}

VOID Trace(TRACE trace, VOID *v)
{
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(CheckRegs),
                     IARG_INST_PTR,
                     IARG_REG_VALUE, REG_AL,
                     IARG_REG_VALUE, REG_AH,
                     IARG_REG_VALUE, REG_AX,
                     IARG_REG_VALUE, REG_EAX,
#if defined(TARGET_IA32E)
                     IARG_REG_VALUE, REG_RAX,
#endif
                     IARG_END
    );

    INS ins = BBL_InsHead(TRACE_BblHead(trace));

    static INT32 origCount = 0;
    if (INS_IsOriginal(ins))
    {
        origCount++;
    }
    
    static INT32 immcount = 10;
    if (immcount > 0)
    {
        for (UINT32 op = 0; op < INS_OperandCount(ins); op++)
        {
            if (INS_OperandIsImmediate(ins, op))
            {
                std::cerr << "Immediate: " << hex << INS_OperandImmediate(ins, op) << " ins " << INS_Disassemble(ins) << endl;
                immcount--;
            }
        }
    }
#endif
}

int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);
    
    GetVmLock();
    ReleaseVmLock();
    
    INS_AddInstrumentFunction(Instruction, 0);
    TRACE_AddInstrumentFunction(Trace, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
