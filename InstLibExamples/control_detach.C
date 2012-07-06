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
#include <stdio.h>
#include "pin.H"
#include <iostream>
#include "instlib.H"


using namespace INSTLIB;


//
// This test case combines detach.C and control.C
// It calls PIN_Detach() whenever a specified region is reached.
// You can specify regions by "-skip, -start_address, -ppfile" etc.
//


// Track the number of instructions executed
ICOUNT icount;

// Contains knobs and instrumentation to recognize start/stop points
CONTROL control;

VOID Handler(CONTROL_EVENT ev, VOID * v, CONTEXT * ctxt, VOID * ip , VOID * tid)
{
    

    switch(ev)
    {
      case CONTROL_START:
        std::cerr << "Start : Detaching at icount:" << icount.Count() <<  endl;
        PIN_RemoveFiniFunctions();
        PIN_Detach();
        break;
    
      case CONTROL_STOP:
        std::cerr << "Stop" << endl;
        break;

      default:
        ASSERTX(false);
        break;
    }
}

VOID helloWorld(VOID *v)
{
    fprintf(stdout, "Hello world!\n");
}

VOID byeWorld(VOID *v)
{
    fprintf(stdout, "Byebye world!\n");
}


INT32 Usage()
{
    cerr <<
        "This pin tool demonstrates uses CONTROL to identify start points in a program and does a PIN_Detach() at those points. \n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}


int main(int argc, char * argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    icount.Activate();

    // Activate alarm, must be done before PIN_StartProgram
    control.CheckKnobs(Handler, 0);
    
    // Callback function "byeWorld" is invoked
    // right before Pin releases control of the application
    // to allow it to return to normal execution
    PIN_AddDetachFunction(helloWorld, 0);
    PIN_AddDetachFunction(byeWorld, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
