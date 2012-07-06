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

/*! @defgroup ICOUNT
  Instrumentation for counting instruction execution
*/

/*! @ingroup ICOUNT
  The example below can be found in InstLibExamples/icount.C

  \include icount.C
*/
class ICOUNT 
{
  public:
    ICOUNT()
    {
        _maxThreads = 10;
        _active = false;
        _counts = new UINT64[_maxThreads];
        memset(_counts, 0, sizeof(_counts[0]) * _maxThreads);
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
        _counts_withREP = new UINT64[_maxThreads];
        memset(_counts_withREP, 0, sizeof(_counts_withREP[0]) * _maxThreads);
#endif
    };
    
    /*! @ingroup ICOUNT
      @return Number of instructions executed
    */
    UINT64 Count(UINT32 tid = 0) const
    {
        ASSERTX(tid < _maxThreads);
        return _counts[tid];
    }
    
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    /*! @ingroup ICOUNT
      @return Number of instructions executed with REP repitions
    */
    UINT64 CountWithREP(UINT32 tid = 0) const
    {
        ASSERTX(tid < _maxThreads);
        return _counts_withREP[tid];
    }
#endif

    /*! @ingroup ICOUNT
      Set the current count
    */
    VOID SetCount(UINT64 count, UINT32 tid = 0)
    {
        ASSERTX(_active == true);
        ASSERTX(tid < _maxThreads);
        _counts[tid] = count;
    }

    /*! @ingroup ICOUNT
      Activate the counter, must be called before PIN_StartProgram
    */
    VOID Activate()
    {
        ASSERTX(_active == false);
        _active = true;
        TRACE_AddInstrumentFunction(Trace, this);
    }

        

  private:
    static VOID Trace(TRACE trace, VOID * icount)
    {
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE,
                           AFUNPTR(Advance), IARG_ADDRINT, icount, IARG_UINT32, BBL_NumIns(bbl), IARG_THREAD_ID, IARG_END);
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
            for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
            {
            // Count the number of times REP-prefixed instructions are executed.
            // We assume that these instructions always refer memory.
            // The count computation is done by dividing  the size of the 
            // total memory accessed ( obtained dynamically via
            // IARG_MEMORYWRITE(READ)_SIZE) by the size of the memory
            // accessed by the un-prefixed instruction.
                if( INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins) )
                {
                    BOOL write = INS_IsMemoryWrite(ins);
                    UINT32 size = write ? INS_MemoryWriteSize(ins) : INS_MemoryReadSize(ins);

                    if( size == VARIABLE_MEMORY_REFERENCE_SIZE )
                    {
                        UINT32 prim_size = write ? INS_MemoryWriteSizeWithoutPrefix(ins) : INS_MemoryReadSizeWithoutPrefix(ins);
                        if(write)
                            INS_InsertCall(ins,
                                         IPOINT_AFTER,
                                         AFUNPTR(Advance_withREP),
                                         IARG_ADDRINT, icount,
                                         IARG_INST_PTR,
                                         IARG_UINT32, prim_size,
                                         IARG_MEMORYWRITE_SIZE,
                                         IARG_THREAD_ID, 
                                         IARG_END);
                        else
                            INS_InsertCall(ins,
                                         IPOINT_AFTER,
                                         AFUNPTR(Advance_withREP),
                                         IARG_ADDRINT, icount,
                                         IARG_INST_PTR,
                                         IARG_UINT32, prim_size,
                                         IARG_MEMORYREAD_SIZE,
                                         IARG_THREAD_ID, 
                                         IARG_END);
                    }
                }
            }
#endif
        }
    }

    static VOID Advance(ICOUNT * ic, UINT32 c, UINT32 tid)
    {
        // ASSERTX(tid < _maxThreads);
        ic->_counts[tid] += c;
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
        ic->_counts_withREP[tid] += c;
#endif
    }

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    static VOID Advance_withREP(ICOUNT * ic, VOID * ip, UINT32 prim_size, UINT32 size, UINT32 tid)
    {
        UINT32 count = size/prim_size;
        if(count)
        {
            ic->_counts_withREP[tid] += (count -1); 
            // -1 because Advance counted this once
        }
        else
        {
            // The repeat count is ZERO.
            // Should we subtract one to undo Advance()'s counting of
            // this instruction? 
        }
    }
#endif
    
    BOOL _active;
    UINT64 *_counts;
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    UINT64 *_counts_withREP;
#endif
    UINT64 _maxThreads;
};


}
