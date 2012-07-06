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
  Demonstrates api replacement.

  Generates a trace of malloc/free calls

 */

#include "pin.H"
#include <iostream>
#include <fstream>

typedef void *(*MALLOC_FUNPTR)(size_t size);
typedef void  (*FREE_FUNPTR)(void *memblock);
typedef void  (*EXIT_FUNPTR)(int status);

using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "malloctracereplace.out", "specify trace file name");

/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool collects an instruction trace for debugging\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

void * MallocProbe(int size)
{
    // Get a handle to the original malloc so we can call it
    // This handles the case when there are multiple malloc routines
    // Must be first line of routine
    MALLOC_FUNPTR mallocWithoutReplacement;

    mallocWithoutReplacement = (MALLOC_FUNPTR)PIN_RoutineWithoutReplacement();
    
    void * ptr = mallocWithoutReplacement(size);

    TraceFile << "malloc(" << size << ") returns " << ptr << endl;
    return ptr;
}

void FreeProbe(void *p)
{
    FREE_FUNPTR freeWithoutReplacement;

    // Must be first line of routine
    freeWithoutReplacement = (FREE_FUNPTR)PIN_RoutineWithoutReplacement();

    freeWithoutReplacement(p);
    TraceFile << "free(" << p << ")" << endl;
}

void ExitProbe(int code)
{
    EXIT_FUNPTR exitWithoutReplacement;

    // Must be first line of routine
    exitWithoutReplacement = (EXIT_FUNPTR)PIN_RoutineWithoutReplacement();

    TraceFile << "## eof" << endl << flush;
    TraceFile.close();
    
    exitWithoutReplacement(code);
}


/* ===================================================================== */
// Called every time a new image is loaded
// Look for routines that we want to replace
VOID ImageLoad(IMG img, VOID *v)
{
    RTN mallocRtn = RTN_FindByName(img, "malloc");
    if (RTN_Valid(mallocRtn))
    {
        RTN_ReplaceWithUninstrumentedRoutine(mallocRtn, AFUNPTR(MallocProbe));
        TraceFile << "Replaced malloc:" << IMG_Name(img) << endl;
    }
    
    RTN freeRtn = RTN_FindByName(img, "free");
    if (RTN_Valid(freeRtn))
    {
        RTN_ReplaceWithUninstrumentedRoutine(freeRtn, AFUNPTR(FreeProbe));
        TraceFile << "Replaced free:" << IMG_Name(img) << endl;
    }

    RTN exitRtn = RTN_FindByName(img, "exit");
    if (RTN_Valid(exitRtn))
    {
        RTN_ReplaceWithUninstrumentedRoutine(exitRtn, AFUNPTR(ExitProbe));
        TraceFile << "Replaced exit:" << IMG_Name(img) << endl;
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);

    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
