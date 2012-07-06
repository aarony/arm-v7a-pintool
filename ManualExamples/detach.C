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

// This tool shows how to detach Pin from an 
// application that is under Pin's control.

UINT64 icount = 0;

#define N 10000
VOID docount() 
{
    icount++;

    // Release control of application if 10000 
    // instructions have been executed
    if ((icount % N) == 0) 
    {
        PIN_Detach();
    }
}
 
VOID Instruction(INS ins, VOID *v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
}

VOID ByeWorld(VOID *v)
{
    std::cerr << endl << "Detached at icount = " << N << endl;
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    // Callback function to invoke for every 
    // execution of an instruction
    INS_AddInstrumentFunction(Instruction, 0);
    
    // Callback functions to invoke before
    // Pin releases control of the application
    PIN_AddDetachFunction(ByeWorld, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
