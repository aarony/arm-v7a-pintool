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
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "pin.H"

map<ADDRINT,INT32> divisors;

// Value profile for a div instruction
class DIVPROF
{
  public:
    DIVPROF(ADDRINT trace, ADDRINT instruction) : _traceAddress(trace), _instructionAddress(instruction)
    {
        _count = 0;
        memset(_divisors, 0, sizeof(_divisors[0]) * NumDiv);
    };
    static VOID ProfileDivide(DIVPROF * dp, INT32 divisor);
    
  private:
    enum 
    {
        NumDiv = 8,
        OptimizeThreshold = 50
    };

    INT32 CommonDivisor();
    VOID InsertProfile(INS ins);
    
    const ADDRINT _traceAddress;
    const ADDRINT _instructionAddress;
    INT32 _count;
    INT32 _divisors[NumDiv];
};

INT32 DIVPROF::CommonDivisor()
{
    for (INT32 i = 1; i < NumDiv; i++)
    {
        if (_divisors[i] * 2 > _count)
        {
            return i;
        }
    }

    return 0;
}
    
VOID DIVPROF::ProfileDivide(DIVPROF * dp, INT32 divisor)
{
    // Profile the divisors in the interesting range
    if (divisor >= 1 && divisor <= NumDiv)
        dp->_divisors[divisor]++;

    // If we exceeded the execution threshold, decide what to do with this divisor
    dp->_count++;
    if (dp->_count < OptimizeThreshold)
        return;
    
    std::cout << "Instruction exceeded threshold at " << hex << dp->_instructionAddress << endl;

    // If we haven't already made a decision, pick a likely divisor
    // 0 means no divisor
    if (divisors.find(dp->_instructionAddress) == divisors.end())
        divisors[dp->_instructionAddress] = dp->CommonDivisor();

    // Unlink this trace so we will remove the instrumentation and optimize it
    CODECACHE_InvalidateTraceAtProgramAddress(dp->_traceAddress);
}

LOCALFUN VOID InsertProfile(TRACE trace, INS ins)
{
    DIVPROF * dp = new DIVPROF(TRACE_Address(trace), INS_Address(ins));
    
    INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(DIVPROF::ProfileDivide),
                   IARG_PTR, dp, IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_END);
}
        
INT32 CheckOne(ADDRINT divisor)
{
    return divisor != 1;
}

ADDRINT Div64()
{
    return 0;
}

LOCALFUN VOID OptimizeDivide(INS ins, INT32 divisor)
{
    if (divisor == 1)
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Zero), IARG_RETURN_REGS, REG_GDX, IARG_END);
        INS_InsertIfCall(ins, IPOINT_BEFORE, AFUNPTR(CheckOne), IARG_REG_VALUE, INS_OperandReg(ins, 0), IARG_END);
        INS_InsertThenCall(ins, IPOINT_BEFORE, AFUNPTR(Zero), IARG_RETURN_REGS, REG_GDX, IARG_END);
        INS_Delete(ins);
    }
    
}

VOID Trace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            if ((INS_Opcode(ins) == XED::XEDICLASS_IDIV
                 || INS_Opcode(ins) == XED::XEDICLASS_DIV)
                && INS_OperandIsReg(ins, 0))
            {
                map<ADDRINT,INT32>::const_iterator di = divisors.find(INS_Address(ins));

                if (di == divisors.end())
                {
                    // No information, let's profile it
                    std::cout << "Profiling instruction at " << hex << INS_Address(ins) << endl;
                    InsertProfile(trace, ins);
                }
                else if (di->second > 0)
                {
                    // We found a divisor
                    std::cout << "Optimizing instruction at " << hex << INS_Address(ins) << " with value " << di->second << endl;
                    OptimizeDivide(ins, di->second);
                }
                else
                {
                    std::cout << "no optimizing or profile of instruction at " << hex << INS_Address(ins) << " with value " << di->second << endl;
                }

            }
        }
    }
}
            

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    TRACE_AddInstrumentFunction(Trace, 0);
    
    PIN_StartProgram();
    
    return 0;
}
