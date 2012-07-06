/*BEGIN_LEGAL 
#BEGIN_LEGAL
##BEGIN_LEGAL
##INTEL CONFIDENTIAL
##Copyright 2002-2005 Intel Corporation All Rights Reserved.
##
##The source code contained or described herein and all documents
##related to the source code (Material) are owned by Intel Corporation
##or its suppliers or licensors. Title to the Material remains with
##Intel Corporation or its suppliers and licensors. The Material may
##contain trade secrets and proprietary and confidential information of
##Intel Corporation and its suppliers and licensors, and is protected by
##worldwide copyright and trade secret laws and treaty provisions. No
##part of the Material may be used, copied, reproduced, modified,
##published, uploaded, posted, transmitted, distributed, or disclosed in
##any way without Intels prior express written permission.  No license
##under any patent, copyright, trade secret or other intellectual
##property right is granted to or conferred upon you by disclosure or
##delivery of the Materials, either expressly, by implication,
##inducement, estoppel or otherwise. Any license under such intellectual
##property rights must be express and approved by Intel in writing.
##
##Unless otherwise agreed by Intel in writing, you may not remove or
##alter this notice or any other notice embedded in Materials by Intel
##or Intels suppliers or licensors in any way.
##END_LEGAL
#INTEL CONFIDENTIAL
#Copyright 2002-2005 Intel Corporation All Rights Reserved.
#
#The source code contained or described herein and all documents
#related to the source code (Material) are owned by Intel Corporation
#or its suppliers or licensors. Title to the Material remains with
#Intel Corporation or its suppliers and licensors. The Material may
#contain trade secrets and proprietary and confidential information of
#Intel Corporation and its suppliers and licensors, and is protected by
#worldwide copyright and trade secret laws and treaty provisions. No
#part of the Material may be used, copied, reproduced, modified,
#published, uploaded, posted, transmitted, distributed, or disclosed in
#any way without Intels prior express written permission.  No license
#under any patent, copyright, trade secret or other intellectual
#property right is granted to or conferred upon you by disclosure or
#delivery of the Materials, either expressly, by implication,
#inducement, estoppel or otherwise. Any license under such intellectual
#property rights must be express and approved by Intel in writing.
#
#Unless otherwise agreed by Intel in writing, you may not remove or
#alter this notice or any other notice embedded in Materials by Intel
#or Intels suppliers or licensors in any way.
#END_LEGAL
Intel Open Source License 

Copyright (c) 2002-2005 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
#include "common.h"
namespace INSTLIB 
{
/*! @defgroup TIME_WARPER
  It is often desirable to use instrumentation to change behavior of program
  in certain ways so that different runs of the program (with the same input)
  are same. Time_warper allows uses to modify non-repitative constructs
  such as instructions reading the cycle counters and system calls reading
  time of the day.
*/


/*! @defgroup TIME_WARPER_RDTSC
  @ingroup TIME_WARPER
  Modify the behaviors of RDTSC instruction on IA32 and IA32E
*/

/*! @ingroup TIME_WARPER_RDTSC
*/
class TIME_WARP_RDTSC
{
  public:
    TIME_WARP_RDTSC():_disableKnob(KNOB_MODE_WRITEONCE, "pintool", "no_rdtsc_warp", KNOB_ONLY_ON_MAC, "Do not modify the behavior of RDTSC instructions on x86")
    {
    }

    bool IsActive()
    {
        return (!_disableKnob);
        return true;
    }

    /*! @ingroup TIME_WARPER_RDTSC
      Activate the controller if the -length knob is provided
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(VOID * val)
    {
        if (_disableKnob)
            return 0;
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
   // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(ProcessRDTSC, this);
#endif
        return 1;
    }

  private:
    KNOB<BOOL>  _disableKnob;
    UINT64 _edx_eax;
    UINT32 _eax;
    UINT32 _edx;

    static UINT32 SwizzleEdx(TIME_WARP_RDTSC *rd)
    {
        rd->_edx = (rd->_edx_eax & 0xffffffff00000000ULL) >> 32;
        // cerr << "SwizzleEdx() returning 0x"<< hex << edx << endl;
        return rd->_edx;
    }

    static UINT32 SwizzleEax(TIME_WARP_RDTSC *rd)
    {
        rd->_eax = rd->_edx_eax & 0x00000000ffffffffULL;
        rd->_edx_eax+=100;
        // cerr << "SwizzleEax() edx_eax= 0x"<< hex << edx_eax << endl;
        // cerr << "SwizzleEax() returning 0x"<< hex << eax << endl;
        return rd->_eax;
    }
    static VOID PrintEaxEdx(ADDRINT reax, ADDRINT redx)
    {
        cerr << "PrintEaxEdx():reg eax = 0x"<< hex << reax << endl;
        cerr << "PrintEaxEdx():reg edx = 0x"<< hex << redx << endl;
    }

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    // Pin calls this function every time a new trace is encountered
    // Goal: Make rdtsc repeatable  across runs
    // NOTE: We are using TRACE instrumentation because it has higher
    // precedence than INS instrumentation.
    static VOID ProcessRDTSC(TRACE trace, VOID *v)
    {
        TIME_WARP_RDTSC  * rd = static_cast<TIME_WARP_RDTSC *>(v);
        rd->_edx_eax = 0x1ULL;
        rd->_eax= 0;
        rd->_edx= 0;

        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
            {

                if(INS_IsRDTSC(ins))
                {
                    //RDTSC
                    // INS_InsertCall(ins, IPOINT_AFTER,
                       // (AFUNPTR)PrintEaxEdx, IARG_REG_VALUE, REG_GAX,IARG_REG_VALUE, REG_GDX,  IARG_END);
                    INS_InsertCall(ins, IPOINT_AFTER,
                       (AFUNPTR)SwizzleEdx,IARG_ADDRINT, rd,  IARG_RETURN_REGS, REG_GDX, IARG_END);
                    INS_InsertCall(ins, IPOINT_AFTER,
                       (AFUNPTR)SwizzleEax,IARG_ADDRINT, rd, IARG_RETURN_REGS, REG_GAX, IARG_END);
                    // INS_InsertCall(ins, IPOINT_AFTER,
                       // (AFUNPTR)PrintEaxEdx, IARG_REG_VALUE, REG_GAX,IARG_REG_VALUE, REG_GDX,  IARG_END);
                }
            }
        }
    }
#endif
};

/*! @ingroup TIME_WARPER_MULTI
*/
class TIME_WARP
{
  public:
    /*! @ingroup TIME_WARPER_MULTI
    */
    /*! @ingroup TIME_WARPER_MULTI
      Activate all the component controllers
    */
    INT32 CheckKnobs(VOID * val)
    {
        _val = val;
        INT32 start = 0;
        start = start + _rdtsc.CheckKnobs(this);
        return start;
    }
    bool RDTSC_modified() { return _rdtsc.IsActive(); };

  private:
    VOID * _val;

    TIME_WARP_RDTSC _rdtsc;
};
}
