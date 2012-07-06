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

VOID ImageLoad(IMG img, VOID * v)
{
    cout << "Tool loading " << IMG_Name(img) << " at " << IMG_LoadOffset(img) << endl;
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        cout << "  sec " << SEC_Name(sec) << " " << SEC_Address(sec) << ":" << SEC_Size(sec) << endl;
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            cout << "    rtn " << RTN_Name(rtn) << " " << RTN_Address(rtn) << ":" << RTN_Size(rtn) << endl;

            RTN_Open(rtn);
            
            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                // Just print first and last
                if (!INS_Valid(INS_Prev(ins)) || !INS_Valid(INS_Next(ins)))
                {
                    cout << "      " << INS_Address(ins);

#if 0
                    cout << " " << INS_Disassemble(ins) << " read:";
                    
                    for (UINT32 i = 0; i < INS_MaxNumRRegs(ins); i++)
                    {
                        cout << " " << REG_StringShort(INS_RegR(ins, i));
                    }
                    cout << " writes:";
                    for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
                    {
                        cout << " " << REG_StringShort(INS_RegW(ins, i));
                    }
#endif                    

                    cout <<  endl;
                }
            }

            RTN_Close(rtn);
        }
    }
}

VOID ImageUnload(IMG img, VOID * v)
{
    cout << "Tool unloading " << IMG_Name(img) << " at " << IMG_LoadOffset(img) << endl;
}

VOID Trace(TRACE trace, VOID *v)
{
    INS head = BBL_InsHead(TRACE_BblHead(trace));
    
    INT32 line;
    INT32 column;
    const CHAR * file;
    PIN_FindColumnLineFileByAddress(INS_Address(head), &column, &line, &file);
    if (file)
    {
        cout << file << ":" << dec << line << ":" << column << " " << hex;
    }

    RTN rtn = RTN_FindByAddress(INS_Address(head));

    if (RTN_Valid(rtn))
        cout << IMG_Name(SEC_Img(RTN_Sec(rtn)))
             << ":"
             << RTN_Name(rtn)
             << "+"
             << INS_Address(head) - RTN_Address(rtn)
             << " "
             << INS_Disassemble(head)
             << endl;
}

int main(INT32 argc, CHAR **argv)
{
    cout << hex;
    cout.setf(ios::showbase);
    
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    IMG_AddUnloadFunction(ImageUnload, 0);
    TRACE_AddInstrumentFunction(Trace, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
