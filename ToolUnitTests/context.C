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
#include "xed-iclass.H"
#include <iostream>
using namespace std;
using namespace XED;

ADDRINT BeforeIP = 0x0;

#define COUNT 10
UINT64 icount = 0;


LOCALFUN VOID PrintContext(const CONTEXT * ctxt)
{
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    cout << "ip:    " << PIN_GetContextReg( ctxt, REG_EIP ) << endl;
    cout << "gax:   " << PIN_GetContextReg( ctxt, REG_GAX ) << endl;
    cout << "gbx:   " << PIN_GetContextReg( ctxt, REG_GBX ) << endl;
    cout << "gcx:   " << PIN_GetContextReg( ctxt, REG_GCX ) << endl;
    cout << "gdx:   " << PIN_GetContextReg( ctxt, REG_GDX ) << endl;
    cout << "gsi:   " << PIN_GetContextReg( ctxt, REG_GSI ) << endl;
    cout << "gdi:   " << PIN_GetContextReg( ctxt, REG_GDI ) << endl;
    cout << "gbp:   " << PIN_GetContextReg( ctxt, REG_GBP ) << endl;
    cout << "sp:    " << PIN_GetContextReg( ctxt, REG_ESP ) << endl;

    cout << "ss:    " << PIN_GetContextReg( ctxt, REG_SEG_SS ) << endl;
    cout << "cs:    " << PIN_GetContextReg( ctxt, REG_SEG_CS ) << endl;
    cout << "ds:    " << PIN_GetContextReg( ctxt, REG_SEG_DS ) << endl;
    cout << "es:    " << PIN_GetContextReg( ctxt, REG_SEG_ES ) << endl;
    cout << "fs:    " << PIN_GetContextReg( ctxt, REG_SEG_FS ) << endl;
    cout << "gs:    " << PIN_GetContextReg( ctxt, REG_SEG_GS ) << endl;
    cout << "gflags:" << PIN_GetContextReg( ctxt, REG_GFLAGS ) << endl;
    cout << endl;
    
#endif
}


VOID SetBeforeContext(VOID * ip, const CONTEXT * ctxt)
{
    BeforeIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_EIP );
}


VOID ShowAfterContext(VOID * ip, const CONTEXT * ctxt)
{
    ADDRINT AfterIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_EIP );

    if ( BeforeIP == AfterIP )
        cout << "Failure!! Before Context IP = 0x" << BeforeIP << ", After Context IP = 0x " << AfterIP << endl;
    else
        cout << "Success!! Before Context IP = 0x" << BeforeIP << ", After Context IP = 0x " << AfterIP << endl;
}


VOID ShowTakenContext(VOID * ip, const CONTEXT * ctxt)
{
    ADDRINT TakenIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_EIP );

    if ( BeforeIP == TakenIP )
        cout << "Failure!! Before Context IP = 0x" << BeforeIP << ", Taken Context IP = 0x " << TakenIP << endl;
    else
        cout << "Success!! Before Context IP = 0x" << BeforeIP << ", Taken Context IP = 0x " << TakenIP << endl;
}


VOID Instruction(INS ins, VOID * v)
{
    xed_iclass_t iclass = (xed_iclass_t)INS_Opcode(ins);

    if (iclass >= XEDICLASS_JO && iclass <= XEDICLASS_JNLE)
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(SetBeforeContext),
                       IARG_INST_PTR,
                       IARG_CONTEXT,
                       IARG_END);

        INS_InsertCall(ins, IPOINT_AFTER,
                       AFUNPTR(ShowAfterContext),
                       IARG_INST_PTR,
                       IARG_CONTEXT,
                       IARG_END);

        INS_InsertCall(ins, IPOINT_TAKEN_BRANCH,
                       AFUNPTR(ShowTakenContext),
                       IARG_INST_PTR,
                       IARG_CONTEXT,
                       IARG_END);
    }
}


int main(int argc, char * argv[])
{
    cout << hex;
    cout.setf(ios::showbase);
    
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
