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

/*! @defgroup FILTER

  Filters are used to select static parts of the program to instrument. A
  user may only want to instrument specific routines or may way to ignore
  shared libraries.

  The example below can be foundin InstLibExamples/filter.C

  \include filter.C
  
*/
  
/*! @defgroup FILTER_RTN
  @ingroup FILTER
  Filter for selecting routines by name
  Use -filter_rtn <name> to select a routine. To select multiple routines, use more than one -filter_rtn.
*/

/*! @ingroup FILTER_RTN
*/
class FILTER_RTN
{
  public:
    FILTER_RTN() : _rtnsKnob(KNOB_MODE_APPEND, "pintool", "filter_rtn", "", "Routines to instrument") 
    {}
    
    /*! @ingroup FILTER_RTN
      Activate the filter. Must be done before PIN_StartProgram
    */
    VOID Activate()
    {
        PIN_InitSymbols();
        _activated = true;
    }
    
    /*! @ingroup FILTER_RTN
      Return true if the filter is not active or the routine that contains this trace is selected
    */
    BOOL SelectTrace(TRACE trace)
    {
        ASSERTX(_activated);
        
        if (!RTN_Valid(TRACE_Rtn(trace)))
        {
            if (_rtnsKnob.NumberOfValues() > 0)
                return false;
            else
                return true;
        }
        
        return SelectRtn(TRACE_Rtn(trace));
    }

    /*! @ingroup FILTER_RTN
      Return true if the filter is not active or the routine is selected
    */
    BOOL SelectRtn(RTN rtn)
    {
        ASSERTX(RTN_Valid(rtn));
        ASSERTX(_activated);

        UINT32 numRtns = _rtnsKnob.NumberOfValues();
        
        // No rtn based selection
        if (numRtns == 0)
            return true;

        // RTN must be on list for selection
        for (UINT32 i = 0; i < numRtns; i++)
        {
            if (RTN_Name(rtn) == _rtnsKnob.Value(i))
                return true;
        }

        return false;
    }
 
  private:
    BOOL _activated;
    KNOB<string> _rtnsKnob;

};

/*! @defgroup FILTER_LIB
  @ingroup FILTER
  Filter for selecting shared libraries
  Use -filter_no_shared_libs to ignore all shared libraries
*/

/*! @ingroup FILTER_LIB
*/
class FILTER_LIB
{
  public:
    FILTER_LIB() : _noSharedLibKnob(KNOB_MODE_WRITEONCE, "pintool", "filter_no_shared_libs", "", "Do not instrument shared libraries") 
    {}

    /*! @ingroup FILTER_LIB
      Activate the filter. Must be done before PIN_StartProgram
    */
    VOID Activate()
    {}

    /*! @ingroup FILTER_LIB
      Return true if the filter is not active or the shared library that contains this trace is selected
    */
    BOOL SelectTrace(TRACE trace)
    {
        if (_noSharedLibKnob.Value()
            && (!RTN_Valid(TRACE_Rtn(trace))
                || IMG_Type(SEC_Img(RTN_Sec(TRACE_Rtn(trace)))) == IMG_TYPE_SHAREDLIB))
            return false;

        return true;
    }

  private:
    KNOB<BOOL> _noSharedLibKnob;
};


/*! @defgroup FILTER_MULTI
  @ingroup FILTER

  Filter that includes all the filters
  See @ref FILTER_RTN, @ref FILTER_LIB
*/

/*! @ingroup FILTER_MULTI
*/
class FILTER
{
  public:
    /*! @ingroup FILTER_MULTI
      Activate the filter. Must be done before PIN_StartProgram
    */
    VOID Activate()
    {
        _filterRtn.Activate();
        _filterLib.Activate();
    }

    /*! @ingroup FILTER_MULTI
      Return true if the filter is not active or the this trace is selected
    */
    BOOL SelectTrace(TRACE trace)
    {
        if (!_filterRtn.SelectTrace(trace)
            || !_filterLib.SelectTrace(trace)
        )
            return false;

        return true;
    }
    
  private:
    FILTER_RTN _filterRtn;
    FILTER_LIB _filterLib;
};

}
