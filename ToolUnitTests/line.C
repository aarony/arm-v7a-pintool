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

FILE * trace;

#if defined(TARGET_MAC)
#define MAINNAME "_main"
#else
#define MAINNAME "main"
#endif

VOID ImageLoad(IMG img, VOID * v)
{
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            if (RTN_Name(rtn) == MAINNAME)
            {
                const CHAR * path;
                INT32 line;
                PIN_FindLineFileByAddress(RTN_Address(rtn), &line, &path);

                if (path)
                {
                    const CHAR * file = strrchr(path,'h');
                    
                    fprintf(trace,"File %s line %d\n", file, line);
                }
            }
        }
    }
}

VOID Fini(INT32 code, VOID *)
{
    fclose(trace);
}

int main(INT32 argc, CHAR **argv)
{
    trace = fopen("line.output", "w");

    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
