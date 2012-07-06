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

/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Vijay Janapa Reddi
*/

/* ===================================================================== */
/*! @file
  Replace an original function with a custom function defined in the tool. The
  new function can have either the same or different signature from that of its
  original function.
*/

/* ===================================================================== */
#include "pin.H"
#include <iostream>

using namespace std;

/* ===================================================================== */
typedef VOID * (*FUNCPTR_MALLOC)(size_t);

/* ===================================================================== */
VOID * NewMalloc(FUNCPTR_MALLOC orgFuncptr, UINT32 arg0, ADDRINT returnIp)
{
    cout << "NewMalloc (" << hex << (ADDRINT) orgFuncptr << ", " 
                          << dec << arg0 << ", " 
                          << hex << returnIp << ")" 
                          << endl << flush;

    VOID * v = orgFuncptr(arg0);
    return v;
}

/* ===================================================================== */
VOID ImageLoad(IMG img, VOID *v)
{
    cout << IMG_Name(img) << endl;

    RTN rtn = RTN_FindByName(img, "malloc");
    if (RTN_Valid(rtn))
    {
        cout << "Replacing malloc in " << IMG_Name(img) << endl;

        RTN_ReplaceSignatureProbed(rtn, AFUNPTR(NewMalloc), IARG_ORG_FUNCPTR,
                                             IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                                             IARG_RETURN_IP,
                                             IARG_END);
    }
}

/* ===================================================================== */
int main(INT32 argc, CHAR *argv[])
{
    PIN_InitSymbols();

    PIN_Init(argc, argv);

    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    PIN_StartProbedProgram();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
