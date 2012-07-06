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
#include "pin.H"
#include <iostream>

UINT64 icount = 0;

LOCALFUN VOID PrintContext(CONTEXT * ctxt)
{
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    cout << "gax:   " << ctxt->_gax << endl;
    cout << "gbx:   " << ctxt->_gbx << endl;
    cout << "gcx:   " << ctxt->_gcx << endl;
    cout << "gdx:   " << ctxt->_gdx << endl;
    cout << "gsi:   " << ctxt->_gsi << endl;
    cout << "gdi:   " << ctxt->_gdi << endl;
    cout << "gbp:   " << ctxt->_gbp << endl;
    cout << "sp:    " << ctxt->_stack_ptr << endl;

#if defined(TARGET_IA32E)
    cout << "r8:    " << ctxt->_r8 << endl;
    cout << "r9:    " << ctxt->_r9 << endl;
    cout << "r10:   " << ctxt->_r10 << endl;
    cout << "r11:   " << ctxt->_r11 << endl;
    cout << "r12:   " << ctxt->_r12 << endl;
    cout << "r13:   " << ctxt->_r13 << endl;
    cout << "r14:   " << ctxt->_r14 << endl;
    cout << "r15:   " << ctxt->_r15 << endl;
#endif
    
    cout << "ss:    " << ctxt->_ss << endl;
    cout << "cs:    " << ctxt->_cs << endl;
    cout << "ds:    " << ctxt->_ds << endl;
    cout << "es:    " << ctxt->_es << endl;
    cout << "fs:    " << ctxt->_fs << endl;
    cout << "gs:    " << ctxt->_gs << endl;
    cout << "gflags:" << ctxt->_gflags << endl;

    cout << "mxcsr: " << ctxt->_fxsave._mxcsr << endl;
    
#endif
}


VOID ShowContext(VOID * ip, VOID * handle, ADDRINT gax)
{
    CONTEXT ctxt;
    
    // Capture the context. This must be done first before some floating point
    // registers have been overwritten
    PIN_MakeContext(handle, &ctxt);

    static bool first = false;

    if (first)
    {
        cout << "ip:    " << ip << endl;
    
        PrintContext(&ctxt);

        cout << endl;
    }

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    ASSERTX(gax == ctxt._gax);
#endif    
}

VOID Trace(TRACE tr, VOID *v)
{
    TRACE_InsertCall(tr, IPOINT_BEFORE, AFUNPTR(ShowContext), IARG_INST_PTR, IARG_CONTEXT, IARG_REG_VALUE, REG_GAX, IARG_END);
}

int main(int argc, char * argv[])
{
    cout << hex;
    cout.setf(ios::showbase);
    
    PIN_Init(argc, argv);

    TRACE_AddInstrumentFunction(Trace, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
