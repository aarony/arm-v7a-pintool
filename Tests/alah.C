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
unsigned int n = 0xdeadbeef;
unsigned int res = 0;

VOID ahal()
{ 
#if defined(PIN_GNU_COMPATIBLE)
   asm("mov %0,%%eax; mov %%ah,%%al; mov %%eax,%1" : "=r"(res) : "r"(n));
#elif defined(PIN_MS_COMPATIBLE)
   __asm {
       mov eax, [n]
       mov al, ah
       mov [res], eax
  }
#endif	
}
    
VOID Check(INT32 code, VOID *v)
{
    if (res != 0xdeadbebe)
    {
        printf("n %x res %x\n",n,res);
        exit(1);
    }
}

VOID Instruction(INS ins, VOID *v)
{
    static bool first = true;
    if (first)
    {
        INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)ahal, IARG_END);
        INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)Check, IARG_END);
    }
    first = false;
}


int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
