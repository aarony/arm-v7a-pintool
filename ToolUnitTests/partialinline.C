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

const UINT32 period = 10000;

// The running bbl count is kept here
UINT32 acount = 0;
UINT32 pcount = 0;

INT32 Always()
{
    ++acount;
    return acount==period;
}

VOID Rare()
{
    ++pcount;
    acount = 0;
}

UINT32 ccount = 0;
UINT32 rcount = 0;

INT32 AlwaysNoinline()
{
    ++ccount;
    if (ccount == 1000000) printf("Should not get here\n");
    return ccount==period;
}

VOID RareNoinline()
{
    ++rcount;
    if (ccount == 1000000) printf("Should not get here\n");
    ccount = 0;
}

UINT32 bcount = 0;
UINT32 qcount = 0;

VOID Noinline()
{
    ++bcount;

    if (bcount==period)
    {
        ++qcount;
        bcount = 0;
    }
}

// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Always()->Rare() are partially inlined
        BBL_InsertIfCall(bbl,   IPOINT_BEFORE, (AFUNPTR)Always, IARG_END);
        BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR)Rare, IARG_END);

        // Always()->Rare() are partially inlined
        BBL_InsertIfCall(bbl,   IPOINT_BEFORE, (AFUNPTR)AlwaysNoinline, IARG_END);
        BBL_InsertThenCall(bbl, IPOINT_BEFORE, (AFUNPTR)RareNoinline, IARG_END);

        // Noinline() is not inlined
        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)Noinline, IARG_END);
    }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    if (pcount*period+acount != qcount*period+bcount
        || pcount*period+acount != rcount*period+ccount)
    {
        fprintf(stderr, "Counts NOT matched:\n"
                "partial-inline   count=%d (pcount=%d acount=%d),\n"
                "partial-noinline count=%d (rcount=%d ccount=%d),\n"
                "noninline        count=%d (qcount=%d bcount=%d)\n",
                pcount*period+acount, pcount, acount,
                rcount*period+ccount, rcount, ccount,
                qcount*period+bcount, qcount, bcount);
        exit(1);
    }
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{
    // Initialize pin
    PIN_Init(argc, argv);

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
