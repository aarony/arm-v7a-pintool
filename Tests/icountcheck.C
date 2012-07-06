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
#include <stdlib.h>
#include <iostream>
#include "pin.H"

#if 0
#define DBG_PRINT(a) (a)
#else
#define DBG_PRINT(a)
#endif

UINT64 count_ins = 0;
UINT64 count_bbl_ins = 0;

VOID docount_ins(void * pc)
{
    count_ins++;
    DBG_PRINT(printf("Anal: docount_ins: %lld at pc %p\n", count_ins, pc));
}

VOID docount_bbl_ins(void * pc, INT32 icount)
{
    count_bbl_ins += icount;
    DBG_PRINT(printf("Anal: docount_bbl_ins(%d): %lld at pc %p\n", icount, count_bbl_ins, pc));
    if (count_ins != count_bbl_ins)
    {
        std::cerr << "mismatch: count_ins " << count_ins << " != count_bbl_ins "
                  << count_bbl_ins << " at pc " << pc << endl;
        exit(1);
    }
}
    
VOID Trace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        DBG_PRINT(printf("Inst: Sequence address %p\n",(CHAR*)(INS_Address(BBL_InsHead(bbl)))));
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            DBG_PRINT(printf("Inst:   %p\n",(CHAR*)(INS_Address(ins))));
            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(docount_ins), IARG_INST_PTR, IARG_END);
        }
        
        INT32 icount = BBL_NumIns(bbl);
        DBG_PRINT(printf("Inst:     -> control flow change (bbl size %d)\n", icount));
        INS_InsertCall(BBL_InsTail(bbl), IPOINT_BEFORE, AFUNPTR(docount_bbl_ins), IARG_INST_PTR, IARG_UINT32, icount, IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v)
{
    std::cerr << "Count (ins) " << count_ins << ",  (bbl_ins) " << count_bbl_ins << endl;
}

int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
