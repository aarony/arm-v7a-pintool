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
// Mark Charney

/* ===================================================================== */
/*! @file
 *  This file contains an IA32/EM64T XED useage example.
 */

#include "pin.H"
#include "xed-interface.H"
using namespace XED;
#include <iostream>

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */


/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool prints IA32/EM64T instructions"
         << endl;

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */

VOID use_xed(ADDRINT pc)
{

#if defined(TARGET_IA32)
    const xed_state_t dstate( XED_MACHINE_MODE_LEGACY_32, ADDR_WIDTH_32b, ADDR_WIDTH_32b );
#elif defined(TARGET_IA32E)
    const xed_state_t dstate( XED_MACHINE_MODE_LONG_64, ADDR_WIDTH_64b );
#else
# error  not supported on this platform. IA32/EM64T only.
#endif
    xed_decoded_inst_t xedd(dstate);

    //FIXME: pass in the proper length...
    const unsigned int max_inst_len = 15;
    xed_error_enum_t xed_code = xed_decode(&xedd, reinterpret_cast<UINT8*>(pc), max_inst_len);
    BOOL xed_ok = (xed_code == XED_ERROR_NONE);
    if (xed_ok)
    {
        cerr << hex << std::setw(8) << pc << " ";
        xedd.print_short( cerr, pc);
        cerr << endl;
    }

}

/* ===================================================================== */

VOID Instruction(INS ins, VOID *v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)use_xed, 
                   IARG_INST_PTR,
                   IARG_END);
}

/* ===================================================================== */


/* ===================================================================== */

int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    INS_AddInstrumentFunction(Instruction, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
