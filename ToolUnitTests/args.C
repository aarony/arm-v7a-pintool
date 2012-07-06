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
#include <fstream>

VOID PrintArgs(INT32 arg1, INT32 arg2, INT32 arg3, INT32 arg4, INT32 arg5, INT32 arg6, INT32 arg7, INT32 arg8)
{ 
    ofstream out("args.output");
    out << arg1 << endl;
    out << arg2 << endl;
    out << arg3 << endl;
    out << arg4 << endl;
    out << arg5 << endl;
    out << arg6 << endl;
    out << arg7 << endl;
    out << arg8 << endl;
}
    
VOID CheckIp(ADDRINT ip1,
             ADDRINT ip2,
             ADDRINT ip3,
             ADDRINT ip4,
             ADDRINT ip5,
             ADDRINT ip6,
             ADDRINT ip7,
             ADDRINT ip8,
             ADDRINT ip9,
             ADDRINT ip10
)
{
    ASSERTX(ip1 == ip2);
    ASSERTX(ip1 == ip3);
    ASSERTX(ip1 == ip4);
    ASSERTX(ip1 == ip5);
    ASSERTX(ip1 == ip6);
    ASSERTX(ip1 == ip7);
    ASSERTX(ip1 == ip8);
    ASSERTX(ip1 == ip9);
    ASSERTX(ip1 == ip10);
}

    
VOID Instruction(INS ins, VOID *v)
{
    static INT32 count = 0;
    
    if (count % 64 == 0)
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CheckIp),
                       IARG_INST_PTR,
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_ADDRINT, INS_Address(ins),
                       IARG_END);
    }
    

    if (count == 0)
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(PrintArgs),
                       IARG_UINT32, 1,
                       IARG_UINT32, 2,
                       IARG_UINT32, 3,
                       IARG_UINT32, 4,
                       IARG_UINT32, 5,
                       IARG_UINT32, 6,
                       IARG_UINT32, 7,
                       IARG_UINT32, 8,
                       IARG_END);
    }

    count++;
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
