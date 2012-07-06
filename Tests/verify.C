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

VOID * lastInstPtr;
BOOL isPredictedTaken;
VOID * predictedInstPtr;
UINT64 icount = 0;
UINT64 errors = 0;
BOOL isSkipped = TRUE; // always skip checking the first inst

VOID CountError()
{
    errors++;
    if (errors > 100)
    {
        std::cerr << "Too many errors, giving up\n";
        exit(errors);
    }
}

VOID CheckFlow(VOID * instPtr, INT32 isTaken, VOID * fallthroughAddr, VOID * takenAddr)
{
    isPredictedTaken = isTaken;

    icount++;
    
    //fprintf(stderr,"Current: %p isTaken %d fallthroughAddr %p takenAddr %p\n", instPtr, isTaken, fallthroughAddr, takenAddr);

    if (predictedInstPtr != instPtr && !isSkipped)
    {
        fprintf(stderr,"From: %p predicted InstPtr %p, actual InstPtr %p\n", lastInstPtr, predictedInstPtr, instPtr);
        CountError();
    }
    
    isSkipped = FALSE;
    
    if (isTaken)
    {
        predictedInstPtr = takenAddr;
    }
    else
    {
        predictedInstPtr = fallthroughAddr;
    }

    lastInstPtr = instPtr;
}
    
VOID Taken()
{
    if (!isPredictedTaken)
    {
        fprintf(stderr,"%p taken but not predictedInstPtr\n", lastInstPtr);
        CountError();
    }
}

VOID Skip()
{
    isSkipped = TRUE;
}

VOID Instruction(INS ins, VOID *v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)CheckFlow,
        IARG_INST_PTR,
        IARG_BRANCH_TAKEN,
        IARG_FALLTHROUGH_ADDR,
        IARG_BRANCH_TARGET_ADDR,
        IARG_END);

    if (INS_IsBranchOrCall(ins))
    {
        INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)Taken, IARG_END);
    }

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    if (INS_IsSysenter(ins))
    { // sysenter on x86 has some funny control flow that we can't correctly verify for now
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)Skip, IARG_END);
    }
#endif
}

VOID Fini(INT32 code, VOID *v)
{
    if (code)
    {
        exit(code);
    }
    
    std::cerr << errors << " errors (" << icount << " instructions checked)" << endl;
    exit(errors);
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
