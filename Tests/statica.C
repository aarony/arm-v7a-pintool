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
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include "pin.H"

KNOB<string> KnobInputFile(KNOB_MODE_WRITEONCE, "pintool",
    "i", "<imagename>", "specify an image to read");

INT32 Usage()
{
    cerr <<
        "This tool disassembles an image.\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

int main(INT32 argc, CHAR **argv)
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    IMG img = IMG_Open(KnobInputFile);

    std::cout << hex;
    
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        std::cout << "Section: " << setw(8) << SEC_Address(sec) << " " << SEC_Name(sec) << endl;
        
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            std::cout << "  Rtn: " << setw(8) << RTN_Address(rtn) << " " << RTN_Name(rtn) << endl;

            RTN_Open(rtn);
            
            for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
            {
                std::cout << "    " << setw(8) << INS_Address(ins) << " " << INS_Disassemble(ins) << endl;
            }

            RTN_Close(rtn);
        }
    }
    IMG_Close(img);
}
