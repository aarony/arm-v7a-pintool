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

// unmatched_syscall_ip keeps track of the IP of the last syscall encounted.
// It is set before the syscall and reset afterward. Therefore, if the
// afterward instrumentation is bypassed, which could happen due to the funny
// control flow in sysenter on x86, then we will see a non-zero value in
// unmatched_syscall_ip when we execute the next syscall.

ADDRINT unmatched_syscall_ip = 0;

VOID BeforeSyscall(ADDRINT ip)
{
    if (unmatched_syscall_ip != 0)
    {
        printf("AfterSyscall() is not executed after the syscall at %x\n",
               unmatched_syscall_ip);
        fflush(stdout);
    }
    
    unmatched_syscall_ip = ip;
}

VOID AfterSyscall(ADDRINT ip)
{
    unmatched_syscall_ip = 0;
}

VOID Instruction(INS ins, VOID *v)
{
    if (INS_IsSyscall(ins))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)BeforeSyscall, IARG_INST_PTR, IARG_END);
        INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)AfterSyscall, IARG_INST_PTR, IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v)
{
    printf("Finished\n");
    fflush(stdout);
    
    if (code)
    {
        exit(code);
    }
}

int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);
    
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
