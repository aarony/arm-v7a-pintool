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

FILE * out;


// Inspect the sequence, but do not instrument
VOID Trace(TRACE trace, VOID *v)
{
    fprintf(out, "Trace address %p\n",(CHAR*)(TRACE_Address(trace)));

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            fprintf(out, "  %p\n",(CHAR*)(INS_Address(ins)));
        }
    }

    fprintf(out, "\n");
}

int main(INT32 argc, CHAR **argv)
{
    out = fopen("sequence.out", "w");
    
    PIN_Init(argc, argv);
    
    TRACE_AddInstrumentFunction(Trace, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
