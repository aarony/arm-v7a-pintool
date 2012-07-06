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
 *  This file contains a Pintool for sampling the IPs of instruction executed.
 *  It serves as an example of a more efficient way to write analysis routines
 *  that include conditional tests.
 *  Currently, it works on IA32 and EM64T.
 */

#include <stdio.h>
#include "pin.H"

FILE * trace;

const INT32 N = 100000;
const INT32 M =  50000;

INT32 icount = N;

/*
 *  IP-sampling could be done in a single analysis routine like:
 *
 *        VOID IpSample(VOID *ip)
 *        {
 *            --icount;
 *            if (icount == 0)
 *            {
 *               fprintf(trace, "%p\n", ip);
 *               icount = N + rand() % M;
 *            }
 *        }
 *
 *  However, we break IpSample() into two analysis routines,
 *  CountDown() and PrintIp(), to facilitate Pin inlining CountDown()
 *  (which is the much more frequently executed one than PrintIp()).
 */

INT32 CountDown()
{
    --icount;
    return (icount==0);
}


// The IP of the current instruction will be printed and
// the icount will be reset to a random number between N and N+M.
VOID PrintIp(VOID *ip)
{
    fprintf(trace, "%p\n", ip);
    
    // Prepare for next period
    icount = N + rand() % M; // random number from N to N+M
}


// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    // CountDown() is called for every instruction executed
    INS_InsertIfCall(ins, IPOINT_BEFORE, (AFUNPTR)CountDown, IARG_END);
    
    // PrintIp() is called only when the last CountDown() returns a non-zero value.
    INS_InsertThenCall(ins, IPOINT_BEFORE, (AFUNPTR)PrintIp, IARG_INST_PTR, IARG_END);
    
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "#eof\n");
    fclose(trace);
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{
    trace = fopen("isampling.out", "w");
    
    // Initialize pin
    PIN_Init(argc, argv);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
