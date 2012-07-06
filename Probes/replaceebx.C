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
  @ORIGINAL_AUTHOR: Robert Cohn
*/

/* ===================================================================== */
/*! @file
  Checks that functions that reference the spill pointer in their prolog can be replaced
 */



#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

void Replacement()
{
    typeof(Replacement) * original  = (typeof(Replacement)*)PIN_RoutineWithoutReplacement();

    cerr << "In replacement" << endl;
    
    // Get a handle to the original malloc so we can call it
    // This handles the case when there are multiple malloc routines
    // Must be first line of routine
    
    original();
    cerr << "After replacement" << endl;
}

/* ===================================================================== */
// Called every time a new image is loaded
// Look for routines that we want to probe
VOID ImageLoad(IMG img, VOID *v)
{
    RTN ebxmodRtn = RTN_FindByName(img, "ebxmod");
    if (RTN_Valid(ebxmodRtn))
    {
        RTN_ReplaceProbed(ebxmodRtn, AFUNPTR(Replacement));
        cout << "Inserted probe for ebxmod:" << endl;
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    PIN_Init(argc,argv);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    PIN_StartProbedProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
