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
/* ======================================================================= 
 * ORIGINAL AUTHOR: Vijay Janapa Reddi
 *
   ===================================================================== */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "pin.H"

using namespace std;

KNOB<INT32> KnobCnt(KNOB_MODE_WRITEONCE,  "pintool", "c", "500", "x");

ofstream TraceFile("opin1.out");

/* ===================================================================== */
// All exits from trace go back to original code
VOID SetNativeXferForTraceExits(TRACE& trace)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS ins = BBL_InsTail(bbl);

        if (INS_IsBranchOrCall(ins) || INS_IsRet(ins))
            INS_SetNativeXfer(ins);
    }
}

/* ===================================================================== */
VOID ExtendTrace(TRACE Trace, VOID *v)
{
    UINT32 inscnt = 0;

    // Loop till we see a call or uncond. branch or ret to terminate
    for(;;)
    {
        inscnt += BBL_NumIns (TRACE_BblTail(Trace));

        if (inscnt > 70)
            break;

        INS ins = BBL_InsTail( TRACE_BblTail(Trace));

        // Needed because it is possible to run into data
        if (!INS_Valid(ins))
            break;

        if (!INS_HasFallThrough(ins))
            break;

        // Adding the branch edge causes the branch to be inverted
        if (INS_IsDirectBranchOrCall(ins) && !INS_IsCall(ins))
        {
            TRACE_AddFallthroughEdg(Trace);
            // Add the branch target path
            //TRACE_AddBranchEdg(Trace);
        }
        else
        {
            // The fallthrough path
            TRACE_AddFallthroughEdg(Trace);
        }
    }
}

/* ===================================================================== */
BOOL OkToGenerateTrace(TRACE Trace)
{
    for (BBL bbl = TRACE_BblHead(Trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS ins = BBL_InsTail(bbl);

        if (INS_IsCall(ins) && INS_IsDirectBranchOrCall(ins))
            if (INS_DirectBranchOrCallTargetAddress(ins) == INS_NextAddress(ins))
                return FALSE;
    }

    return TRUE;
}

/* ===================================================================== */
VOID RecordMemRead(VOID * ip, VOID * addr)
{   
    TraceFile.setf(ios::showbase);
    TraceFile << hex << ip << " R " << addr << endl;
}

/* ===================================================================== */
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    TraceFile.setf(ios::showbase);
    TraceFile << hex << ip << " W " << addr << endl;
}

VOID EveryIns() 
{ 
    TraceFile << "AnalysisRoutine!" << endl; 
}

/* ===================================================================== */
VOID InstrumentTrace(TRACE Trace)
{
    for (BBL bbl = TRACE_BblHead(Trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) EveryIns, IARG_END); 

            if (INS_IsMemoryRead(ins))
                INS_InsertCall(ins, IPOINT_BEFORE,(AFUNPTR) RecordMemRead, IARG_INST_PTR, IARG_MEMORYREAD_EA, IARG_END); 
            else if (INS_IsMemoryWrite(ins))
                INS_InsertCall(ins, IPOINT_BEFORE,(AFUNPTR) RecordMemWrite, IARG_INST_PTR, IARG_MEMORYWRITE_EA, IARG_END);
        }
    }
}

/* ===================================================================== */
BOOL ValidAddress(ADDRINT Src, ADDRINT Dst)
{
    if (Src > 0x40000000)
        return (Dst > 0x40000000 ? TRUE : FALSE);
    else if (Src > 0x08000000)
        return (Dst > 0x08000000 ? TRUE : FALSE);

    return FALSE;
}

/* ===================================================================== */
VOID Image(IMG img, VOID * v)
{
    if (APP_ImgHead() != img)
        return;

    TraceFile << "IMG_Name " << IMG_Name(img) << endl;

    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        if (! SEC_IsExecutable(sec))
            continue;
        
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            RTN_Open(rtn);

            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                if (!INS_IsDirectBranchOrCall(ins))
                    continue;

                // Patchable branches are those that are 5 bytes wide
                // So here is where compiler information on where it is safe to
                // overwrite instructions is critical to the insertion of probes
                // In this example we are simply looking for points that we
                // know are statically safe to overwrite or modify
                if (! INS_IsPatchable(ins))
                    continue;
        

                ADDRINT SrcAddr = INS_Address(ins);
                ADDRINT DstAddr = INS_DirectBranchOrCallTargetAddress(ins);

                // Linker inserts some bogus addresses for the 
                // targets so cater for that in this AOTI model
                if (!ValidAddress(SrcAddr, DstAddr))
                    continue;

                    static int x = 0;
                    x++;
                    if (x > KnobCnt.Value()) continue;

                TRACE Trace = TRACE_Allocate(DstAddr);
                TraceFile << dec << " X " << x << endl;

                //ExtendTrace(Trace, 0);

                if (OkToGenerateTrace(Trace))
                {
                    TraceFile << hex << "0x" << SrcAddr << " 0x" << DstAddr << endl;

                    SetNativeXferForTraceExits(Trace);
                
                    InstrumentTrace(Trace);

                    // This has to be done if branch targets were added as the
                    // fallthroguh path in the new trace
                    TRACE_StraightenControlFlow(Trace);

                    TRACE_GenerateCode(Trace);

                    // Simply redirect the branch to execute the the trace
                    // rather than its original target in the program
                    INS_RedirectControlFlowToAddress(ins, TRACE_CodeCacheAddress(Trace));
                    TraceFile << "Trace " << (void*)TRACE_CodeCacheAddress(Trace) << endl;
                }

                TRACE_Deallocate(Trace);
            }

            RTN_Close(rtn);
        }
    }
}

/* ===================================================================== */
int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    PIN_InitSymbols();

    IMG_AddInstrumentFunction(Image, 0);
 
    PIN_StartProbedProgram(); 

    return 0;
}
