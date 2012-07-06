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

using namespace std;

#if defined (TARGET_MAC)
// Mac
#define LIBC "libSystem.B.dylib"
#else
// Linux
#define LIBC "libc.so"
#endif

FILE * trace;

BOOL DoneLoad(IMG img)
{
    if (IMG_Type(img) == IMG_TYPE_STATIC)
        return true;

    // Give up after libc.so is loaded
    if (strstr(IMG_Name(img).c_str(), LIBC))
        return true;

    return false;
}
    
INT32 readcount = 0;

VOID Mem(ADDRINT a, ADDRINT s)
{}

VOID ImageLoad(IMG img, VOID * v)
{
    fprintf(trace,"Loading %s\n", IMG_Name(img).c_str());
    fflush(trace);

    // Scan the instructions to test image parsing code
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
#if 0
            fprintf(trace, "Routine name %s %x\n", 
                  RTN_Name(rtn).c_str(),
                  RTN_Address(rtn));
            fflush(trace);
#endif
            RTN_Open(rtn);
            
            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                if( INS_HasMemoryRead2(ins) )
                {
                    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Mem), IARG_MEMORYREAD2_EA, IARG_MEMORYREAD_SIZE, IARG_END);
                }

                if( INS_IsMemoryRead(ins) )
                {
                    //fprintf(trace, "RSize %d %s\n",INS_MemoryReadSize(ins), INS_Disassemble(ins).c_str());
                    
                    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Mem), IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
                }

                if( INS_IsMemoryWrite(ins) )
                {
                    //fprintf(trace, "WSize %d %x %s\n",INS_MemoryWriteSize(ins), INS_Address(ins), INS_Disassemble(ins).c_str());
                    
                    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Mem), IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);
                }
            }

            RTN_Close(rtn);
        }
    }


    if (DoneLoad(img))
    {
        fprintf(trace,"Finished\n");
        fclose(trace);
        exit(0);
    }
}

int main(INT32 argc, CHAR **argv)
{
    // On Mac, ImageLoad() works only after we call PIN_InitSymbols().
    // This is not necessary on Linux, but doing it doesn't hurt.
    PIN_InitSymbols();
    
    trace = fopen("probe.out", "w");

    if( PIN_Init(argc, argv) )
    {
        PIN_ERROR("bad commandline\n");
    }
    
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
