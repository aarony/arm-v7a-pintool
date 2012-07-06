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
#include <fstream>
#include "common.h"
namespace INSTLIB 
{

/*! @ingroup ALARM
   Handler for alarm events
 */
typedef VOID (*ALARM_HANDLER)(VOID *, CONTEXT * ctxt, VOID *, VOID * tid);

/*! @defgroup ALARM_ADDRESS_COUNT
  @ingroup ALARM
  Signal an alarm when the specified address has been executed n times
*/

/*! @ingroup ALARM_ADDRESS_COUNT
*/
class ALARM_ADDRESS_COUNT
{
  public:
    ALARM_ADDRESS_COUNT(BOOL passContext = false) 
        : _count(0),_handler(0), _address(0), _active(false), _armed(false),
          _rearm(false),
          _always_armed(false), 
          _passContext(passContext)
    {};

    /*! @ingroup ALARM_ADDRESS_COUNT
      @return Code address for alarm
    */
    ADDRINT Address() const
    {
        ASSERTX(_active == true);
        return _address;
    }

    /*! @ingroup ALARM_ADDRESS_COUNT
      @return Remaining count for this alarm
    */
    ADDRINT Count() const
    {
        ASSERTX(_active == true);
        return _count;
    }

    /*! @ingroup ALARM_ADDRESS_COUNT
      Turn on an alarm. You should call Activate for this alarm before PIN_StartProgram
      @param count Number of times to execution address before alarm fires
      @param handler Call this function when alarm fires
      @param val Pass this value to the handler when the alarm fires
    */
    VOID SetAlarm(UINT64 count, ALARM_HANDLER handler, VOID * val, BOOL rearm=false, BOOL always_armed=false)
    {
        ASSERTX(_active == true);
        _count = count;
        _orig_count = count;
        _handler = handler;
        _val = val;
        _armed = true;
        _rearm = rearm;
        _always_armed = always_armed;
    }

    /*! @ingroup ALARM_ADDRESS_COUNT
      This function initializes an alarm and must be called before @ref PIN_StartProgram.
      It does turn the alarm on. Use SetAlarm.
      @param address Address for alarm
    */
    VOID Activate(ADDRINT address)
    {
        ASSERTX(_active == false);
        _active = true;
        _address = address;
        INS_AddInstrumentFunction(Ins, this);
    }

    BOOL Active() const { return _active; }
    
  private:
    static VOID Ins(INS ins, VOID * val)
    {
        ALARM_ADDRESS_COUNT *alarm = static_cast<ALARM_ADDRESS_COUNT*>(val);
        
        if (INS_Address(ins) == alarm->_address)
        {
            if (alarm->_passContext)
            {
                INS_InsertCall(ins, IPOINT_BEFORE,
                               AFUNPTR(Advance), 
                               IARG_ADDRINT, alarm, 
                               IARG_CONTEXT, 
                               IARG_INST_PTR, IARG_END);
            }
            else 
            {
                INS_InsertCall(ins, IPOINT_BEFORE,
                               AFUNPTR(Advance), 
                               IARG_ADDRINT, alarm,
                               IARG_ADDRINT, static_cast<ADDRINT>(0), // pass a null instead
                               IARG_INST_PTR, IARG_END);
            }
        }
    }
    
    static VOID Advance(ALARM_ADDRESS_COUNT * ic, CONTEXT * ctxt, VOID * ip)
    {
        if (!ic->_armed)
            return;
        
        BOOL fire = ic->_always_armed;
        if (!fire)
        {
            ic->_count -= 1;
            if (ic->_count <= 0)
            {
                fire = true;
            }
        }

        if (fire)
        { 
            if (ic->_rearm)
            {
                ic->_count = ic->_orig_count;
            }
            else
            {
                ic->_armed = false;
            }

            ic->_handler(ic->_val, ctxt, ip, /*tid==*/0);
        }
    }
    
    INT64 _count;
    INT64 _orig_count;
    ALARM_HANDLER _handler;
    VOID * _val;
    ADDRINT _address;
    BOOL _active;
    BOOL _armed;
    BOOL _rearm;
    BOOL _always_armed;
    BOOL _passContext;
};

    
/*! @defgroup ALARM_SYMBOL_COUNT
  @ingroup ALARM
  Signal an alarm when the specified symbol has been executed n times

  The example below can be found in InstLibExamples/alarm_symbol.C

  \include alarm_symbol.C
*/

/*! @ingroup ALARM_SYMBOL_COUNT
*/
class ALARM_SYMBOL_COUNT
{
  public:
    ALARM_SYMBOL_COUNT(BOOL passContext=false)
        : _symbol(0),
          _active(false),
          _passContext(passContext)
    {
        PIN_InitSymbols();
    }

    /*! @ingroup ALARM_SYMBOL_COUNT
      @return Label for alarm
    */
    const CHAR * Symbol() const
    {
        ASSERTX(_active == true);
        return _symbol;
    }

    /*! @ingroup ALARM_SYMBOL_COUNT
      @return Remaining count for this alarm
    */
    ADDRINT Count() const
    {
        ASSERTX(_active == true);
        return _addressAlarm.Count();
    }

    /*! @ingroup ALARM_SYMBOL_COUNT
      Turn on an alarm. You should call Activate for this alarm before PIN_StartProgram
      @param count Number of times to execution address before alarm fires
      @param handler Call this function when alarm fires
      @param val Pass this value to the handler when the alarm fires
    */
    VOID SetAlarm(UINT64 count, ALARM_HANDLER handler, VOID * val, BOOL rearm=false, BOOL always_armed=false)
    {
        ASSERTX(_active == true);

        if (!_addressAlarm.Active())
        {
            _count = count;
            _handler = handler;
            _val = val;
            _rearm = rearm;
            _always_armed = always_armed;
        }
        else
        {
            _addressAlarm.SetAlarm(count, handler, val, rearm, always_armed);
        }
    }
    
    /*! @ingroup ALARM_SYMBOL_COUNT
      This function initializes an alarm and must be called before @ref PIN_StartProgram.
      It does turn the alarm on. Use SetAlarm.
      @param symbol Name of symbol
    */
    VOID Activate(const CHAR * symbol)
    {
        ASSERTX(_active == false);
        _active = true;
        _symbol = strdup(symbol);
        IMG_AddInstrumentFunction(Img, this);
    }

  private:
    static VOID Img(IMG img, VOID * val)
    {
        ALARM_SYMBOL_COUNT *alarm = static_cast<ALARM_SYMBOL_COUNT*>(val);

        // If we have already activated it, then we can't do it again
        // even if we find another symbol with the same name
        if (alarm->_addressAlarm.Active())
            return;
        
        for( SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym) )
        {
            if (SYM_Name(sym) == alarm->_symbol)
            {
                alarm->_addressAlarm.Activate(SYM_Value(sym) + IMG_LoadOffset(img));
                if (alarm->_handler)
                {
                    alarm->_addressAlarm.SetAlarm(alarm->_count, alarm->_handler, alarm->_val,
                                                  alarm->_rearm, alarm->_always_armed);
                }
            }
        }
    }
    
    ALARM_ADDRESS_COUNT _addressAlarm;
    const CHAR * _symbol;
    UINT64 _count;
    ALARM_HANDLER _handler;
    VOID * _val;
    BOOL _active;
    BOOL _rearm;
    BOOL _always_armed;
    BOOL _passContext;

};
    
/*! @defgroup ALARM_IMAGE_OFFSET_COUNT
  @ingroup ALARM
  Signal an alarm when the specified offset in the specified image has been 
  executed n times

  The example below can be found in InstLibExamples/alarm_image_offset.C

  \include alarm_image_offset.C

*/

/*! @ingroup ALARM_IMAGE_OFFSET_COUNT
*/
class ALARM_IMAGE_OFFSET_COUNT
{
  public:
    ALARM_IMAGE_OFFSET_COUNT(BOOL passContext=false)
        : _image_name(0),
          _active(false),
          _passContext(passContext)
    {
        PIN_InitSymbols();
    }

    /*! @ingroup ALARM_IMAGE_OFFSET_COUNT
      @return Remaining count for this alarm
    */
    ADDRINT Count() const
    {
        ASSERTX(_active == true);
        return _addressAlarm.Count();
    }

    /*! @ingroup ALARM_IMAGE_OFFSET_COUNT
      Turn on an alarm. You should call Activate for this alarm before PIN_StartProgram
      @param count Number of times to execution address before alarm fires
      @param handler Call this function when alarm fires
      @param val Pass this value to the handler when the alarm fires
    */
    VOID SetAlarm(UINT64 count, ALARM_HANDLER handler, VOID * val, BOOL rearm=false, BOOL always_armed=false)
    {
        ASSERTX(_active == true);

        if (!_addressAlarm.Active())
        {
            _count = count;
            _handler = handler;
            _val = val;
            _rearm = rearm;
            _always_armed = always_armed;
        }
        else
        {
            _addressAlarm.SetAlarm(count, handler, val, rearm, always_armed);
        }
    }
    
    /*! @ingroup ALARM_IMAGE_OFFSET_COUNT
      This function initializes an alarm and must be called before @ref PIN_StartProgram.
      It does turn the alarm on. Use SetAlarm.
      @param symbol Name of symbol
    */
    VOID Activate(const CHAR * image_name, UINT64 offset)
    {
        ASSERTX(_active == false);
        _active = true;
        _image_name = strdup(image_name);
        _offset = offset;
        IMG_AddInstrumentFunction(Img, this);
    }

  private:
    static VOID Img(IMG img, VOID * val)
    {
        ALARM_IMAGE_OFFSET_COUNT *alarm = static_cast<ALARM_IMAGE_OFFSET_COUNT*>(val);

        // If we have already activated it, then we can't do it again
        // even if we find another symbol with the same name
        if (alarm->_addressAlarm.Active())
            return;
        if (alarm->_image_name !=  IMG_Name(img))
            return;
        
        cerr << "Activating alarm " << IMG_Name(img) << " + 0x" << hex << alarm->_offset << " IMG_LowAddress(img) == 0x" << hex << IMG_LowAddress(img) << endl;
        alarm->_addressAlarm.Activate(IMG_LowAddress(img) + alarm->_offset);
        if (alarm->_handler)
        {
            alarm->_addressAlarm.SetAlarm(alarm->_count, alarm->_handler, alarm->_val,
                                          alarm->_rearm, alarm->_always_armed);
        }
    }
    
    ALARM_ADDRESS_COUNT _addressAlarm;
    const CHAR * _image_name;
    UINT64 _offset;
    UINT64 _count;
    ALARM_HANDLER _handler;
    VOID * _val;
    BOOL _active;
    BOOL _rearm;
    BOOL _always_armed;
    BOOL _passContext;
};

/*! @defgroup ALARM_ICOUNT
  @ingroup ALARM
  Signal an alarm when the specified number of instructions has been executed

  The example below can be found in InstLibExamples/alarm_icount.C

  \include alarm_icount.C

*/

/*! @ingroup ALARM_ICOUNT
*/
class ALARM_ICOUNT
{
  public:
    ALARM_ICOUNT(BOOL passContext=false)
        : _count(0),
          _active(false),
          _armed(false),
          _passContext(passContext)
    {}
    
    /*! @ingroup ALARM_ICOUNT
      @return Remaining count for this alarm
    */
    INT64 Count() const
    {
        ASSERTX(_active == true);
        return _count;
    }

    /*! @ingroup ALARM_ICOUNT
      Turn on an alarm. You should call Activate for this alarm before PIN_StartProgram
      @param count Number of times to execution address before alarm fires
      @param handler Call this function when alarm fires
      @param val Pass this value to the handler when the alarm fires
    */
    VOID SetAlarm(INT64 count, ALARM_HANDLER handler, VOID * val, BOOL rearm=false, BOOL always_armed=false)
    {
        ASSERTX(_active == true);
        _count = count;
        _handler = handler;
        _armed = true;
        _val = val;
        _rearm = rearm;
        _always_armed = always_armed;
    }

    /*! @ingroup ALARM_ICOUNT
      This function initializes an alarm and must be called before @ref PIN_StartProgram.
      It does turn the alarm on. Use SetAlarm.
    */
    VOID Activate()
    {
        ASSERTX(_active == false);
        _active = true;
        TRACE_AddInstrumentFunction(Trace, this);
    }


    /*! @ingroup ALARM_ICOUNT
      De-activate the counter, should call PIN_RemoveInstrumentation()
      after de-activating
    */
    VOID DeActivate()
    {
        _active = false;
    }
        
    /*! @ingroup ALARM_ICOUNT
      Is this ALARM_ICOUNT active?
    */
    BOOL Active() const { return _active; }

  private:
    static VOID Trace(TRACE trace, VOID * icount)
    {
        ALARM_ICOUNT * ic = static_cast<ALARM_ICOUNT *>(icount);
        if (!ic->_active) return;
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            if (ic->_passContext)
            {
                INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE,
                               AFUNPTR(Advance),
                               IARG_ADDRINT, icount, 
                               IARG_UINT32, BBL_NumIns(bbl), 
                               IARG_CONTEXT, 
                               IARG_INST_PTR, IARG_END);
            }
            else 
            {
                INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE,
                               AFUNPTR(Advance), 
                               IARG_ADDRINT, icount, 
                               IARG_UINT32, BBL_NumIns(bbl),
                               IARG_ADDRINT, static_cast<ADDRINT>(0), // pass a null instead
                               IARG_INST_PTR, IARG_END);
            }
        }
    }
    
    static VOID Advance(ALARM_ICOUNT * ic, INT32 c, CONTEXT * ctxt, VOID * ip)
    {
        if (!ic->_armed)
            return;
        
        BOOL fire = ic->_always_armed;
        if (!fire)
        {
            ic->_count -= c;
            if (ic->_count <= 0)
            {
                fire = true;
            }
        }

        if (fire)
        {
            if (ic->_rearm)
            {
               ic->_count = ic->_orig_count;
            }
            else 
            {
                ic->_armed = false;
            }
            ic->_handler(ic->_val, ctxt, ip, /*tid==*/0);
        }
    }
    
    INT64 _count;
    INT64 _orig_count;
    VOID * _val;
    ALARM_HANDLER _handler;
    BOOL _active;
    BOOL _armed;
    BOOL _rearm;
    BOOL _always_armed;
    BOOL _passContext;
};


} //namespace
