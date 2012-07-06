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
  Similar to Probes/malloctrace.C, but puts replacement functions in the
  application namespace
 */



#include "pin.H"
#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <string.h>

using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "malloctrace2.out", "specify trace file name");

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


typedef typeof(malloc) * MallocType;
typedef typeof(free) * FreeType;
typedef typeof(dlopen) * DlopenType;
typedef typeof(dlsym) * DlsymType;

MallocType MallocWrapper = 0;
FreeType FreeWrapper = 0;

DlopenType AppDlopen = 0;
DlsymType AppDlsym = 0;

/* ===================================================================== */
// Called every time a new image is loaded
// Look for routines that we want to probe
VOID ImageLoad(IMG img, VOID *v)
{
    
    if (strstr(IMG_Name(img).c_str(), "libdl.so"))
    {
        TraceFile << "Found libdl.so " << IMG_Name(img) << endl;

        // Get the function pointer for the application dlopen
        RTN dlopenRtn = RTN_FindByName(img, "__dlopen_check");
        ASSERTX(RTN_Valid(dlopenRtn));
        AppDlopen = DlopenType(RTN_Funptr(dlopenRtn));
    
        // Get the function pointer for the application dlsym
        RTN dlsymRtn = RTN_FindByName(img, "dlsym");
        ASSERTX(RTN_Valid(dlsymRtn));
        AppDlsym = DlsymType(RTN_Funptr(dlsymRtn));

        // inject mallocwrappers.so into application by executing application dlopen
        void * mallocTraceHandle = AppDlopen("mallocwrappers.so", RTLD_LAZY);
        ASSERTX(mallocTraceHandle);
            
        // Get function pointers for the wrappers
        MallocWrapper = MallocType(AppDlsym(mallocTraceHandle, "mallocWrapper"));
        FreeWrapper = FreeType(AppDlsym(mallocTraceHandle, "freeWrapper"));
    }
    
    if (strstr(IMG_Name(img).c_str(), "libc.so"))
    {
        TraceFile << "Found libc.so " << IMG_Name(img) << endl;

        ASSERTX(MallocWrapper && FreeWrapper);

        // Replace malloc and free in application libc with wrappers in mallocwrappers.so
        RTN mallocRtn = RTN_FindByName(img, "malloc");
        ASSERTX(RTN_Valid(mallocRtn));
        RTN_ReplaceWithUninstrumentedRoutine(mallocRtn, AFUNPTR(MallocWrapper));
    
        RTN freeRtn = RTN_FindByName(img, "free");
        ASSERTX(RTN_Valid(freeRtn));
        RTN_ReplaceWithUninstrumentedRoutine(freeRtn, AFUNPTR(FreeWrapper));
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
    
    PIN_StartProbedProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
