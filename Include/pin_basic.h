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
//
// @ORIGINAL_AUTHOR: Robert Cohn
//

/*! @file
 *  This file contains the ISA-independent PIN iterface specification
 */

#ifndef PIN_BASIC_H
#define PIN_BASIC_H

#ifdef  __cplusplus
extern "C" {
#endif


extern PINCLIENTINT * PIN_ClientInt;

// Client side functions
extern VOID PIN_StartProgram();
extern VOID PIN_AddInstrumentSequenceFunction(VOID (*fun)(INS, VOID *), VOID *val);
extern VOID PIN_InsertCall(IPOINT action, INS ins, AFUNPTR funptr, ...);
extern VOID PIN_InsertPredicatedCall(IPOINT action, INS ins, AFUNPTR funptr, ...);
extern VOID PIN_AddFiniFunction(VOID (*fun)(INT32, VOID*), VOID *val);

//
// Functions provided by pin
//
// generic
#define INS_Address(ins)  (PIN_ClientInt->_INS_Address(ins))
#define INS_Next(ins)  (PIN_ClientInt->_INS_Next(ins))
#define INS_Prev(ins)  (PIN_ClientInt->_INS_Prev(ins))
#define INS_Invalid()  (PIN_ClientInt->_INS_Invalid())
#define INS_Opr(ins)  (PIN_ClientInt->_InsOpr(ins))
#define INS_Predicate(ins)  (PIN_ClientInt->_INS_Predicate(ins))

// control-flow
#define INS_IsSyscall(ins)  (PIN_ClientInt->_INS_IsSyscall(ins))
#define INS_IsControlFlowChange(ins)  (PIN_ClientInt->_INS_IsControlFlowChange(ins))
#define INS_HasFallThrough(ins)  (PIN_ClientInt->_INS_HasFallThrough(ins))
#define INS_FallThroughAddress(ins)  (PIN_ClientInt->_INS_FallThroughAddress(ins))
#define INS_HasStaticTarget(ins)  (PIN_ClientInt->_INS_HasStaticTarget(ins))
#define INS_StaticTargetAddress(ins)  (PIN_ClientInt->_INS_StaticTargetAddress(ins))

// memory
#define INS_IsMemoryRead(ins)  (PIN_ClientInt->_INS_IsMemoryRead(ins))
#define INS_IsMemoryWrite(ins)  (PIN_ClientInt->_INS_IsMemoryWrite(ins))
// all currently supported ISAs have same memory size for read/write, so
// we dispatch to a shared call
#define INS_MemoryReadSize(ins)  (PIN_ClientInt->_INS_MemorySize(ins))
#define INS_MemoryWriteSize(ins)  (PIN_ClientInt->_INS_MemorySize(ins))

#define PIN_ArrayBaseRoot  (PIN_ClientInt->_ArrayBaseRoot)

#define PIN_AInsertCall  (PIN_ClientInt->_PIN_AInsertCall)

#define OPR_Next(n)  (PIN_ClientInt->_OprNext(n))
#define OPR_Invalid()  (PIN_ClientInt->_OprInvalid())

#ifdef  __cplusplus
} // extern "C"
#endif

#endif // PIN_H
