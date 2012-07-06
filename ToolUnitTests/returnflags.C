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

#include "pin.H"

// Add the flags you want to set to FLAGSET.
// Note: Modifying the eflags will very likely break your application.
//       Use at your own risk.

#define FLAGSET 0x00000000
//#define FLAGSET 0x00000010 // set AF (less likely to break the application)

ADDRINT SetFlags(ADDRINT app_flags)
{
    //cout << "SetFlags: app_flags = " << hexstr(app_flags) << endl;
    return (app_flags | FLAGSET);
}
    
VOID CheckFlags(ADDRINT app_flags)
{
    if ((app_flags & FLAGSET) != FLAGSET)
    {
        cerr << "inline failed: app_flags= " << hexstr(app_flags) << endl;
        exit(-1);
    }
}

int a[10];
int n = 10;

ADDRINT SetFlagsNoInline(ADDRINT app_flags)
{
    for (int i = 0; i < n; i++)
    {
        a[i] = i;
    }
    return (app_flags | FLAGSET);    
}
    
VOID CheckFlagsNoInline(ADDRINT app_flags)
{
    if ((app_flags & FLAGSET) != FLAGSET)
    {
        cerr << "Noinline failed: app_flags= " << hexstr(app_flags) << endl;
        exit(-1);
    }
}


VOID Instruction(INS ins, VOID *v)
{
    static INT32 count = 0;

    switch(count)
    {
      case 0:
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetFlags),
                       IARG_REG_VALUE, REG_GFLAGS,
                       IARG_RETURN_REGS, REG_GFLAGS, IARG_END);
        break;

      case 1:
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CheckFlags), IARG_REG_VALUE, REG_GFLAGS, IARG_END);
        break;

      case 2:
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SetFlagsNoInline),
                       IARG_REG_VALUE, REG_GFLAGS,
                       IARG_RETURN_REGS, REG_GFLAGS, IARG_END);
        break;
        
      case 3:
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CheckFlagsNoInline), IARG_REG_VALUE, REG_GFLAGS, IARG_END);
        break;
        
      default:
        break;
    }
    
    count++;
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
