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
 * This is an ia32e-specific test which verifies that the stack is propertly aligned in
 * analysis routines and call-backs.
 */

#include "pin.H"

extern "C" VOID CheckSPAlign();

static VOID InstrumentTrace(TRACE trace, VOID *v);
static VOID AtTraceOutOfLine();
static INT32 AtTraceIf();
static VOID AtTraceThenOutOfLine();
static VOID AtEnd(INT32 code, VOID *v);


int main(INT32 argc, CHAR **argv)
{
    CheckSPAlign();
    PIN_Init(argc, argv);

    TRACE_AddInstrumentFunction(InstrumentTrace, 0);
    PIN_AddFiniFunction(AtEnd, 0);

    PIN_StartProgram();
    return 0;
}

static VOID InstrumentTrace(TRACE trace, VOID *v)
{
    CheckSPAlign();

    static int testNum = 0;
    switch (testNum++)
    {
      case 0:
        // Test an out-of-line analysis call.
        //
        TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(AtTraceOutOfLine), IARG_END);
        break;

      case 1:
        // Test an out-of-line "if/then" call.
        //
        TRACE_InsertIfCall(trace, IPOINT_BEFORE, AFUNPTR(AtTraceIf), IARG_END);
        TRACE_InsertThenCall(trace, IPOINT_BEFORE, AFUNPTR(AtTraceOutOfLine), IARG_END);
        break;

#if 0
      // These tests are disabled because Pin cannot yet inline analysis routines that
      // use xmm registers.

      case 2:
        // Test an inlined analysis call.
        //
        TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(CheckSPAlign), IARG_END);
        break;

      case 3:
        // Test an "if/then" call where the "then" is inlined.
        //
        TRACE_InsertIfCall(trace, IPOINT_BEFORE, AFUNPTR(AtTraceIf), IARG_END);
        TRACE_InsertThenCall(trace, IPOINT_BEFORE, AFUNPTR(CheckSPAlign), IARG_END);
        break;
#endif

      default:
        testNum = 0;
        break;
    }
}

static VOID AtTraceOutOfLine()
{
    CheckSPAlign();
}

static INT32 AtTraceIf()
{
    CheckSPAlign();
    return 1;
}

static VOID AtEnd(INT32 code, VOID *v)
{
    CheckSPAlign();
}
