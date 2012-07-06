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
/* --------------------------------------------------------------------------- */
/* mypthreadtool:                                                              */
/* instrumentation hooks for PIN to call into the custom pthread library       */
/* --------------------------------------------------------------------------- */

#ifndef PTHREAD_TOOL_H
#define PTHREAD_TOOL_H

#include "PthreadSim.h"
#include "PthreadCond.h"
#include "PthreadCondAttr.h"
#include "PthreadMutex.h"
#include "PthreadMutexAttr.h"
#include "PthreadOnce.h"
#include "PthreadAttr.h"

namespace PinPthread
{

    /* -------------------------------------------------------------- */
    /* Global Variables                                               */
    /* -------------------------------------------------------------- */
    
    PthreadSim* pthreadsim;
#if 0
    [FIXME] add knob to let user pick stack size of new threads
    KNOB<UINT32> KnobStackSize(KNOB_MODE_WRITEONCE, "pintool",
                               "-stacksize", "0x10000", "Default Thread Stack Size");
#endif
    
    /* -------------------------------------------------------------- */
    /* Function Declarations                                          */
    /* -------------------------------------------------------------- */

    VOID  Init(UINT32, char**);
    VOID  Fini(INT32, VOID*);
    VOID  FlagImgUnload(IMG, VOID*);
    VOID  FlagImg(IMG, VOID*);
    VOID  FlagRtn(RTN, VOID*);
    VOID  FlagTrace(TRACE, VOID*);
    int   CallPthreadAttrDestroy(ADDRINT);
    int   CallPthreadAttrGetdetachstate(ADDRINT, ADDRINT);
    int   CallPthreadAttrGetstack(ADDRINT, ADDRINT, ADDRINT);
    int   CallPthreadAttrGetstackaddr(ADDRINT, ADDRINT);
    int   CallPthreadAttrGetstacksize(ADDRINT, ADDRINT);
    int   CallPthreadAttrInit(ADDRINT);
    int   CallPthreadAttrSetdetachstate(ADDRINT, ADDRINT);
    int   CallPthreadAttrSetstack(ADDRINT, ADDRINT, ADDRINT);
    int   CallPthreadAttrSetstackaddr(ADDRINT, ADDRINT);
    int   CallPthreadAttrSetstacksize(ADDRINT, ADDRINT);
    int   CallPthreadCancel(ADDRINT);
    VOID  CallPthreadCleanupPop(CONTEXT*, ADDRINT);
    VOID  CallPthreadCleanupPush(ADDRINT, ADDRINT);
    int   CallPthreadCondattrDestroy(ADDRINT);
    int   CallPthreadCondattrInit(ADDRINT);
    int   CallPthreadCondBroadcast(ADDRINT);
    int   CallPthreadCondDestroy(ADDRINT);
    int   CallPthreadCondInit(ADDRINT, ADDRINT);
    int   CallPthreadCondSignal(ADDRINT);
    VOID  CallPthreadCondTimedwait(CHECKPOINT*, ADDRINT, ADDRINT, ADDRINT);
    VOID  CallPthreadCondWait(CHECKPOINT*, ADDRINT, ADDRINT);
    VOID  CallPthreadCreate(CONTEXT*, CHECKPOINT*, ADDRINT, ADDRINT, ADDRINT, ADDRINT);
    int   CallPthreadDetach(ADDRINT);
    int   CallPthreadEqual(ADDRINT, ADDRINT);
    VOID  CallPthreadExit(CONTEXT*, ADDRINT);
    int   CallPthreadGetattr(ADDRINT, ADDRINT);
    VOID* CallPthreadGetspecific(ADDRINT);
    VOID  CallPthreadJoin(CHECKPOINT*, CONTEXT*, ADDRINT, ADDRINT);
    int   CallPthreadKeyCreate(ADDRINT, ADDRINT);
    int   CallPthreadKeyDelete(ADDRINT);
    int   CallPthreadKill(ADDRINT, ADDRINT);
    int   CallPthreadMutexattrDestroy(ADDRINT);
    int   CallPthreadMutexattrGetkind(ADDRINT, ADDRINT);
    int   CallPthreadMutexattrInit(ADDRINT);
    int   CallPthreadMutexattrSetkind(ADDRINT, ADDRINT);
    int   CallPthreadMutexDestroy(ADDRINT);
    int   CallPthreadMutexInit(ADDRINT, ADDRINT);
    VOID  CallPthreadMutexLock(CHECKPOINT*, ADDRINT);
    int   CallPthreadMutexTrylock(ADDRINT);
    int   CallPthreadMutexUnlock(ADDRINT);
    VOID  CallPthreadOnce(CONTEXT*, ADDRINT, ADDRINT);
    pthread_t CallPthreadSelf();
    int   CallPthreadSetcancelstate(ADDRINT, ADDRINT);
    int   CallPthreadSetcanceltype(ADDRINT, ADDRINT);
    int   CallPthreadSetspecific(ADDRINT, ADDRINT);
    VOID  CallPthreadTestcancel(CONTEXT*);
    VOID  ProcessCall(ADDRINT, ADDRINT, ADDRINT, BOOL);
    VOID  ProcessReturn(const string*);
    INT32 InMtMode();
    VOID  DoContextSwitch(CHECKPOINT*);
    VOID  WarnNYI(const string*);
    VOID  PrintRtnName(const string*);
    VOID  DummyFunc(void*) {}
    
} // namespace PinPthread

#endif  // #ifndef PTHREAD_TOOL_H
