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
 * See "syncasyncapp.c" for a description of this test.
 */

#include <iostream>
#include "pin.H"


static void InstrumentImage(IMG, VOID *);
static void AtSegvBbl();
static void AtApplicationEnd(INT32, VOID *);

static int ExecuteCount = 0;


int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    PIN_InitSymbols();
    IMG_AddInstrumentFunction(InstrumentImage, 0);
    PIN_AddFiniFunction(AtApplicationEnd, 0);
    PIN_StartProgram();
    return 0;
}


static void InstrumentImage(IMG img, VOID *dummy)
{
    RTN rtn = RTN_FindByName(img, "MakeSegv");
    if (RTN_Valid(rtn))
    {
        // There should only be one block in MakeSegv() that is big.  This is the one that
        // causes the SEGV.
        //
        RTN_Open(rtn);
        for (BBL bbl = RTN_BblHead(rtn);  BBL_Valid(bbl);  bbl = BBL_Next(bbl))
        {
            if (BBL_NumIns(bbl) > 10)
            {
                INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE, (AFUNPTR)AtSegvBbl,
                    IARG_END);
            }
        }
        RTN_Close(rtn);
    }
}


static void AtSegvBbl()
{
    // Sanity check to make sure the tool really instruments something.
    //
    ExecuteCount++;

    // Just eat up time here.  Our goal is to delay long enough to ensure that the
    // application's VTALRM signal get delivered.
    //
    unsigned long val = 123456789;
    for (unsigned long i = 1;  i < 100000000;  i++)
        val = val / i + i;

    volatile unsigned long useResult = val;
}


static void AtApplicationEnd(INT32 code, VOID *dummy)
{
    if (ExecuteCount != 1)
    {
        cerr << "Test did not find MakeSegv() block (count = " << ExecuteCount << ")\n";
        exit(1);
    }
}
