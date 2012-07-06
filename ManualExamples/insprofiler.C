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
// Pin tool that interacts with the profiler by passing the object address

#include <stdio.h>
#include "pin.H"
#include <typeinfo>

// include interface to profile
#include "insprofiler.H"

// pointer to profiler object - will be constructed in main
Profiler *prof;

VOID Analysis(VOID *val)
{
    prof = (Profiler *)(val);
    prof->Hit();
}

VOID Instruction(INS ins, VOID *val)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) Analysis, IARG_PTR, val, IARG_END);
}

VOID Fini(INT32 code, VOID *val)
{
    prof = (Profiler *)(val);
    prof->Stats();
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    // Create a Profiler Object
    Profiler *prof = new Profiler();

    // Passing profiler object to the instrumentation function
    INS_AddInstrumentFunction(Instruction, prof);

    // Passing profiler object to the finish function
    PIN_AddFiniFunction(Fini, prof);
    
    PIN_StartProgram();
    
    return 0;
}
