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
/*! @defgroup CONTROLLER
  It is often desirable to use instrumentation to observe an interval of
  the execution of a program. Controllers are used to detect the beginning
  or end of an interval. Some of the methods are instruction counts, or the
  nth time an address or symbol is executed.

  A controller is usually an ALARM with some coordination for stop and start and
  some built in command line switches.

  The example below can be found in InstLibExamples/control.C

  \include control.C

*/

/*! @ingroup CONTROLLER
  Event that is passed to handler when controller detects the beginning or end of an interval
*/
typedef enum 
{
    CONTROL_INVALID,
    CONTROL_START,///< Beginning of interval
    CONTROL_STOP  ///< End of interval
} CONTROL_EVENT;

/*! @ingroup CONTROLLER
  Type for generic event handler
*/

typedef VOID (*CONTROL_HANDLER)(CONTROL_EVENT, VOID *, CONTEXT *, VOID *, VOID *);


/*! @defgroup CONTROLLER_LENGTH
  @ingroup CONTROLLER
  Controller for detecting the end of an interval using instruction count
  Use -length <n> to capture n instructions
*/

/*! @ingroup CONTROLLER_LENGTH
*/
class CONTROL_LENGTH
{
  public:
    CONTROL_LENGTH(BOOL passContext=false) 
        : _lengthKnob(KNOB_MODE_WRITEONCE, 
                      "pintool", 
                      "length", 
                      "0",
                      "Number of instructions to execute before stopping"),
          _alarmIcount(passContext)
    {}

    /*! @ingroup CONTROLLER_LENGTH
      Activate the controller if the -length knob is provided
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        if (_lengthKnob <= 0)
            return 0;

        _controlHandler = ch;
        _controlVal = val;
        _alarmIcount.Activate();

        return 0;
    }

    /*! @ingroup CONTROLLER_LENGTH
      Notify the controller about a start event. It counts instructions until the end
      of the interval
    */
    VOID Event(CONTROL_EVENT ev)
    {
        if (_lengthKnob <= 0)
            return;

        switch(ev)
        {
          case CONTROL_START:
            _alarmIcount.SetAlarm(_lengthKnob, Stop, this);
            break;

          case CONTROL_STOP:
            break;
            
          default:
            ASSERTX(false);
        }
    }
            
  private:
    static VOID Stop(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_LENGTH * cl = static_cast<CONTROL_LENGTH*>(val);

        cl->_controlHandler(CONTROL_STOP, cl->_controlVal, ctxt, ip, tid);
    }

    KNOB<INT64> _lengthKnob;
    ALARM_ICOUNT _alarmIcount;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
};


/*! @defgroup CONTROLLER_START_ADDRESS
  @ingroup CONTROLLER
  Controller for detecting the start of an interval using an address or symbol and a count.
  Use -start_address [address|address:count|symbol|symbol:count|address:count]
*/

/*! @ingroup CONTROLLER_START_ADDRESS
*/
class CONTROL_START_ADDRESS
{
  public:
    CONTROL_START_ADDRESS(BOOL passContext=false) 
        : _passContext(passContext),
          _startAddress(KNOB_MODE_WRITEONCE, 
                        "pintool",
                        "start_address",
                        "0",
                        "Address and count to trigger a start (e.g. 0x400000, main, memcpy:2, /lib/tls/libc.so.6+0x1563a:1)")
    {
    }
    
    /*! @ingroup CONTROLLER_START_ADDRESS
      Activate the controller if the -start_address knob is provided
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        if (_startAddress.Value() == "0")
            return 0;

        ADDRESS_COUNT  addressCount = ParseAddressCount(_startAddress);
        //cerr << "[START] ";  addressCount.print();

        if (addressCount.name != "")
        {
            if(addressCount.offset == 0)
            {
                _symbolAlarm = new ALARM_SYMBOL_COUNT(_passContext);
                _symbolAlarm->Activate(addressCount.name.c_str());
                _symbolAlarm->SetAlarm(addressCount.count, Start, this, 
                                       addressCount.rearm, addressCount.always_enabled);
            }
            else
            {
                cerr << "Image " << addressCount.name 
                     << " Offset:0x" << hex << addressCount.offset
                     << " Count: " << dec << addressCount.count << endl;
                _image_offset_Alarm = new ALARM_IMAGE_OFFSET_COUNT(_passContext);
                _image_offset_Alarm->Activate(addressCount.name.c_str(), addressCount.offset);
                _image_offset_Alarm->SetAlarm(addressCount.count, Start, this,
                                              addressCount.rearm, addressCount.always_enabled);
            }
        }
        else
        {
            cerr << "Address: 0x" << hex <<  addressCount.address
                 << " Count: " << dec << addressCount.count << endl;
            _addressAlarm = new ALARM_ADDRESS_COUNT(_passContext);
            _addressAlarm->Activate(addressCount.address);
            _addressAlarm->SetAlarm(addressCount.count, Start, this,
                                    addressCount.rearm, addressCount.always_enabled);
        }

        _controlHandler = ch;
        _controlVal = val;

        return 1;
    }

  private:
    static VOID Start(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_START_ADDRESS * cs = static_cast<CONTROL_START_ADDRESS*>(val);

        // Notify the parent
        cs->_controlHandler(CONTROL_START, cs->_controlVal, ctxt, ip, tid);
    }
    
    BOOL _passContext;
    KNOB<string> _startAddress;

    ALARM_ADDRESS_COUNT * _addressAlarm;
    ALARM_SYMBOL_COUNT * _symbolAlarm;
    ALARM_IMAGE_OFFSET_COUNT * _image_offset_Alarm;

    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
};

/*! @defgroup CONTROLLER_STOP_ADDRESS
  @ingroup CONTROLLER
  Controller for detecting the end of an interval using an address or symbol and a count
  Use -stop_address [address|address:count|symbol|symbol:count|address:count]
*/

/*! @ingroup CONTROLLER_STOP_ADDRESS
*/
class CONTROL_STOP_ADDRESS
{
  public:
    CONTROL_STOP_ADDRESS(BOOL passContext=false)
        : _passContext(passContext),
          _stopAddress(KNOB_MODE_WRITEONCE, "pintool", "stop_address", "0", "Address and count to trigger a stop (e.g. 0x400000, main, memcpy:2, /lib/tls/libc.so.6+0x1563a:1)"),
          _addressAlarm(0),
          _symbolAlarm(0)
    {}
    
    /*! @ingroup CONTROLLER_STOP_ADDRESS
      Activate the controller if the -stop_address knob is provided
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        if (_stopAddress.Value() == "0")
            return 0;


        //ParseAddressCount(_stopAddress, &name, &address, &_count, &offset);
        ADDRESS_COUNT  addressCount = ParseAddressCount(_stopAddress);
        _count = addressCount.count;
        _rearm = addressCount.rearm;
        _always_enabled = addressCount.always_enabled;
        //cerr << "[STOP] ";        addressCount.print();


        if (addressCount.name != "")
        {
            if(addressCount.offset == 0)
            {
                _symbolAlarm = new ALARM_SYMBOL_COUNT(_passContext);
                _symbolAlarm->Activate(addressCount.name.c_str());
            }
            else
            {
                cerr << "Image " << addressCount.name
                     << " Offset:0x" << hex << addressCount.offset
                     << " Count: " << dec << _count << endl;
                _image_offset_Alarm = new ALARM_IMAGE_OFFSET_COUNT(_passContext);
                _image_offset_Alarm->Activate(addressCount.name.c_str(), 
                                              addressCount.offset);
            }
        }
        else
        {
                cerr << "Address: 0x" << addressCount.address
                     << " Count: " << dec << _count << endl;
                _addressAlarm = new ALARM_ADDRESS_COUNT(_passContext);
                _addressAlarm->Activate(addressCount.address);
        }

        _controlHandler = ch;
        _controlVal = val;

        return 0;
    }

    /*! @ingroup CONTROLLER_STOP_ADDRESS
      Notify the controller about a start event. It uses the address/count to find the end
      of the interval
    */
    VOID Event(CONTROL_EVENT ev)
    {
        switch(ev)
        {
          case CONTROL_START:
            if (_symbolAlarm)
                _symbolAlarm->SetAlarm(_count, Stop, this, _rearm, _always_enabled);
            else if (_addressAlarm)
                _addressAlarm->SetAlarm(_count, Stop, this, _rearm, _always_enabled);
            else if (_image_offset_Alarm)
                _image_offset_Alarm->SetAlarm(_count, Stop, this, _rearm, _always_enabled);
            break;

          case CONTROL_STOP:
            break;
            
          default:
            ASSERTX(false);
        }
    }
            
  private:
    static VOID Stop(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_STOP_ADDRESS * cs = static_cast<CONTROL_STOP_ADDRESS*>(val);

        // Notify the parent
        cs->_controlHandler(CONTROL_STOP, cs->_controlVal, ctxt, ip, tid);
    }

    BOOL _passContext;
    KNOB<string> _stopAddress;

    UINT64 _count;
    BOOL _always_enabled;
    BOOL _rearm;
    ALARM_ADDRESS_COUNT * _addressAlarm;
    ALARM_SYMBOL_COUNT * _symbolAlarm;
    ALARM_IMAGE_OFFSET_COUNT * _image_offset_Alarm;

    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
};

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

/*! @defgroup CONTROLLER_FINI
  @ingroup CONTROLLER
  Controller ends interval when program exits, not controlled by a switch
*/

/*! @ingroup CONTROLLER_FINI
*/
class CONTROL_FINI
{
  public:
    CONTROL_FINI()
    {}
    
    /*! @ingroup CONTROLLER_FINI
      Always active, sends stop at the fini if we are in an interval
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        _controlHandler = ch;
        _controlVal = val;

        PIN_AddFiniFunction(Fini, this);
        return 0;
    }

    /*! @ingroup CONTROLLER_FINI
      Notify the controller about a start event.
    */
    VOID Event(CONTROL_EVENT ev)
    {
        _ev = ev;
    }
            
  private:
    static VOID Fini(INT32 code, VOID * val)
    {
        CONTROL_FINI * cs = static_cast<CONTROL_FINI*>(val);

        // If we are in an interval, end it
        if (cs->_ev == CONTROL_START)
            cs->_controlHandler(CONTROL_STOP, cs->_controlVal, NULL, NULL, NULL);

    }
    
    CONTROL_EVENT _ev;

    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
};

/*! @defgroup CONTROLLER_SKIP
  @ingroup CONTROLLER
  Controller for counting instructions to skip before beginning an interval.
  Use -skip <n> to skip n instructions before starting an interval.
*/

/*! @ingroup CONTROLLER_SKIP
*/
class CONTROL_SKIP
{
  public:
    CONTROL_SKIP(BOOL passContext=false)
        : _skipKnob(KNOB_MODE_WRITEONCE, "pintool", "skip", "0", "Number of instructions to skip from beginning") ,
          _alarmIcount(passContext)
    {}
    
    /*! @ingroup CONTROLLER_SKIP
      Activate the controller if the -skip knob is provided
      @return true if controller can start an interval
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        _controlHandler = ch;
        _controlVal = val;
        
        if (_skipKnob <= 0)
            return 0;
        
        _alarmIcount.Activate();
        _alarmIcount.SetAlarm(_skipKnob, Start, this);
        return 1;
    }

  private:
    static VOID Start(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_SKIP * cs = static_cast<CONTROL_SKIP*>(val);

        // Notify the parent
        cs->_controlHandler(CONTROL_START, cs->_controlVal, ctxt, ip, tid);
    }
    
    KNOB<INT64> _skipKnob;
    ALARM_ICOUNT _alarmIcount;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
};

/*! @defgroup CONTROLLER_UNIFORM
  @ingroup CONTROLLER
  Controller for periodically counting instructions to skip before 
  beginning an interval.
  Use -uniform_period <n> to skip n instructions before starting an interval.
  Use -uniform_length <m> to end  the interval after m instructions.
*/

/*! @ingroup CONTROLLER_SKIP
*/
class CONTROL_UNIFORM
{
  public:
    CONTROL_UNIFORM(BOOL passContext=false) 
        : _periodKnob(KNOB_MODE_WRITEONCE, "pintool", "uniform_period", "0", "Number of instructions to skip periodically") ,
          _lengthKnob(KNOB_MODE_WRITEONCE, "pintool", "uniform_length", "0", "Number of instructions to capture periodically"),
          _alarmPeriodIcount(passContext),
          _alarmLengthIcount(passContext)

    {}
    
    /*! @ingroup CONTROLLER_UNIFORM
      Activate the controller if the -uniform_period  knob is provided
      @return true if controller can start an interval
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        _controlHandler = ch;
        _controlVal = val;
        
        if (_periodKnob <= 0 && _lengthKnob <= 0 )
            return 0;
       
        if ( _periodKnob <= _lengthKnob )
        {
            cerr << "Uniform period (" << _periodKnob << ") must be greater than uniform length (" << _lengthKnob << ")" << endl;
            ASSERTX(0);
        }
        _active = true;
        _alarmPeriodIcount.Activate();
        _alarmLengthIcount.Activate();
        _alarmPeriodIcount.SetAlarm(_periodKnob, Start, this);
        return 1;
    }
    bool IsActive() const { return _active; };

  private:
    static VOID Start(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_UNIFORM * cu = static_cast<CONTROL_UNIFORM*>(val);

        // Set alarm for the end of this interval
        cu->_alarmLengthIcount.SetAlarm(cu->_lengthKnob, Stop, val);
        // Set alarm for the beginning of the next interval
        cu->_alarmPeriodIcount.SetAlarm(cu->_periodKnob, Start, val);
        // Notify the parent
        cu->_controlHandler(CONTROL_START, cu->_controlVal, ctxt, ip, tid);
    }
    static VOID Stop(VOID * val, CONTEXT * ctxt, VOID * ip, VOID * tid)
    {
        CONTROL_UNIFORM * cu = static_cast<CONTROL_UNIFORM*>(val);

        cu->_controlHandler(CONTROL_STOP, cu->_controlVal, ctxt, ip, tid);
    }
    
    KNOB<INT64> _periodKnob;
    KNOB<INT64> _lengthKnob;
    ALARM_ICOUNT _alarmPeriodIcount;
    ALARM_ICOUNT _alarmLengthIcount;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
    bool _active;
};

/*! @defgroup CONTROLLER_INIT
  @ingroup CONTROLLER
  Controller for unconditionally starting at the beginning
*/

/*! @ingroup CONTROLLER_INIT
*/
class CONTROL_INIT
{
  public:
    /*! @ingroup CONTROLLER_INIT
      Activate unconditionally
      @return true if controller can start an interval
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        ch(CONTROL_START, val, NULL, NULL, NULL);
        return 1;
    }
};

#include "pinpoint_control.H"

/*! @defgroup CONTROLLER_MULTI
  @ingroup CONTROLLER

  Controller that includes controllers for -skip -length -start_address -stop_address
  See @ref CONTROLLER_LENGTH, @ref CONTROLLER_SKIP, @ref CONTROLLER_START_ADDRESS, @ref CONTROLLER_STOP_ADDRESS, @ref CONTROLLER_UNIFORM,
  @ref CONTROLLER_PINPOINT
*/

/*! @ingroup CONTROLLER_MULTI
*/
class CONTROL
{
  public:
    /*! @ingroup CONTROLLER_MULTI
      Open outstream
    */
    CONTROL(BOOL passContext= false) 
        : _passContext(passContext),
          _outKnob(KNOB_MODE_WRITEONCE,
                   "pintool",
                   "control_log",
                   "", 
                   "log file for start/stop conditions"),
          _length(passContext),
          _skip(passContext),
          _startAddress(passContext),
          _stopAddress(passContext),
          _pinpoints(passContext),
          _uniform(passContext)
    {
    }
    /*! @ingroup CONTROLLER_MULTI
      Activate all the component controllers
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val)
    {
        _val = val;
        _controlHandler = ch;
        string filename =  _outKnob.Value();

        if (filename != "")
        {
            _outstream = new ofstream(filename.c_str());
            _outstream->setf(ios::showbase);
        }

        // All the controllers call back via our LocalHandler -- which calls back to the
        // pintool which created this controller.
        INT32 start = 0;
        start = start + _skip.CheckKnobs(LocalHandler, this);
        start = start + _length.CheckKnobs(LocalHandler, this);
        start = start + _startAddress.CheckKnobs(LocalHandler, this);
        start = start + _stopAddress.CheckKnobs(LocalHandler, this);
        start = start + _fini.CheckKnobs(LocalHandler, this);

        INT32 ppstart = 0;
        ppstart = _pinpoints.CheckKnobs(LocalHandler, this, _outstream);

        if (ppstart)
        {
            if(start)
            {
                cerr << "Can not combine pinpoints with any other controller!" << endl;
                ASSERTX(0);
            }
            else
                start = ppstart;
        } 


        INT32 ustart = 0;
        ustart = _uniform.CheckKnobs(LocalHandler, this);
        if ( ustart )
        {
            if(start)
            {
                cerr << "Can not combine uniform with any other controller!" << endl;
                ASSERTX(0);
            } 
            else 
                start = ustart;
        }

        // If none of the controllers has a start condition, then start immediately
        if (start == 0)
        {
            start = start + _init.CheckKnobs(LocalHandler, this);
        }

        return start;
    }
    bool PinPointsActive() const { return _pinpoints.IsActive(); };
    bool UniformActive() const { return _uniform.IsActive(); };
    UINT32 NumPp(UINT64 threadid = 0) const { return _pinpoints.NumPp(threadid); };
    UINT32 PP_Phase(UINT32 pp, UINT64 threadid = 0) const { return _pinpoints.PP_Phase(pp);};
    UINT32 PP_Version() const { return _pinpoints.PP_Version();};
    UINT32 PP_Slice(UINT32 pp, UINT64 threadid = 0) const {return _pinpoints.PP_Slice(pp);}; 
    UINT32 PP_WarmupFactor(UINT32 pp, UINT64 threadid = 0) const {return _pinpoints.PP_WarmupFactor(pp);}; 
    UINT32 PP_WeightTimesThousand(UINT32 pp, UINT64 threadid = 0) const {return _pinpoints.PP_WeightTimesThousand(pp);}; 
    UINT32 CurrentPp(UINT64 threadid = 0) const { return _pinpoints.CurrentPp(threadid); };
    UINT32 CurrentPhase(UINT64 threadid = 0) const { return _pinpoints.CurrentPhase(threadid); };
    // double CurrentPpWeight(UINT64 threadid = 0) const { return _pinpoints.CurrentPpWeight(threadid); };
    UINT32 CurrentPpWeightTimesThousand(UINT64 threadid = 0) const { return _pinpoints.CurrentPpWeightTimesThousand(threadid); };
    UINT64 CurrentPpLength(UINT64 threadid = 0) const { return _pinpoints.CurrentPpLength(threadid); };
    UINT64 CurrentPpStartIcount(UINT64 threadid = 0) const { return _pinpoints.CurrentPpStartIcount(threadid); };

  private:
    static VOID LocalHandler(CONTROL_EVENT ev, VOID * val, CONTEXT * ctxt, VOID * ip, VOID *tid)
    {
        CONTROL * control = static_cast<CONTROL*>(val);

        // Notify all interested controls about this event
        control->_length.Event(ev);
        control->_stopAddress.Event(ev);
        control->_fini.Event(ev);

        // Notify the parent
        control->_controlHandler(ev, control->_val, ctxt, ip, tid);
    }

    BOOL _passContext;
    KNOB<string> _outKnob;
    ofstream * _outstream;

    CONTROL_HANDLER _controlHandler;
    VOID * _val;

    CONTROL_INIT _init;
    CONTROL_FINI _fini;
    CONTROL_LENGTH _length;
    CONTROL_SKIP _skip;
    CONTROL_START_ADDRESS _startAddress;
    CONTROL_STOP_ADDRESS _stopAddress;
    CONTROL_PINPOINT _pinpoints;
    CONTROL_UNIFORM _uniform;
};


} //namespace
