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
 *  This file contains an ARM specific test for checking the return value of system calls.
 */
#include <iostream>
#include <fstream>
#include <syscall.h>

#include "pin.H"

using namespace std;

ofstream trace;

// Print syscall number and arguments
VOID SysBefore(ADDRINT ip, ADDRINT num, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2,
               ADDRINT arg3, ADDRINT arg4, ADDRINT arg5)
{
    trace << "@ip 0x" << hex << ip << ": sys call " << dec << num;
    trace << "(0x" << hex << arg0 << ", 0x" << arg1 << ", 0x" << arg2;
    trace << hex << ", 0x" << arg3 << ", 0x" << arg4 << ", 0x" << arg5 << ")" << endl;
}


// Print the return value of the system call
VOID SysAfter( ADDRINT value, INT32 err, UINT32 r0 )
{
    int error = 0;
    ADDRINT neg_one = 0-1;
    
    
    if ( err == 0 ) 
    {
        if ( r0 != value )
            error = 1;
    }
    else
    {
        if ( value != neg_one )
            error = 3;
        if ( err != labs(r0) )
            error = 4;
    }

    if ( error == 0 )
        trace << "Success: r0=0x" << hex << r0 << ", value=0x" << value << ", errno=" << dec << err << endl;
    else 
        trace << "Failure " << error << ": r0=0x" << hex << r0 << ", value=0x" << value << ", errno=" << dec << err <<endl;
}


// Is called for every instruction and instruments syscalls
VOID Instruction(INS ins, VOID *v)
{
    if (INS_IsSyscall(ins))
    {
        // Arguments and syscall number is only available before
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(SysBefore),
                       IARG_INST_PTR, IARG_SYSCALL_NUMBER,
                       IARG_SYSARG_VALUE, 0, IARG_SYSARG_VALUE, 1,
                       IARG_SYSARG_VALUE, 2, IARG_SYSARG_VALUE, 3,
                       IARG_SYSARG_VALUE, 4, IARG_SYSARG_VALUE, 5,
                       IARG_END);

        // return value only available after
        INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(SysAfter),
                       IARG_SYSRET_VALUE, IARG_SYSRET_ERRNO,
                       IARG_REG_VALUE, REG_R0,
                       IARG_END);
    }
}


VOID Fini(INT32 code, VOID *v)
{
    trace << "#eof" << endl;
    trace.close();
}


int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);

    trace.open( "strace.out" );
    if ( ! trace.is_open() ) 
    {
        cout << "Could not open strace.out" << endl;
        exit(1);
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
