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
#include <iostream>

UINT32 * lastEa;
UINT32 lastValue;

VOID CaptureEaAndValue(UINT32 * ea)
{
    lastEa = ea;
    lastValue = *ea;
}

VOID TestValue(VOID * ip, UINT32 val)
{
    if (val != lastValue)
    {
        fprintf(stderr, "Difference IP: %p, EA: %p, register value: %x, memory value: %x\n",
                ip, lastEa, val, lastValue);

        exit(1);
    }
}
    
VOID Instruction(INS ins, VOID *v)
{
    if (INS_Mnemonic(ins) == "MOV"
        && INS_IsMemoryRead(ins)
        && REG_is_gr(INS_RegW(ins, 0)))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CaptureEaAndValue), IARG_MEMORYREAD_EA, IARG_END);
        INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(TestValue), IARG_INST_PTR, IARG_REG_VALUE, INS_RegW(ins, 0), IARG_END);
    }
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
