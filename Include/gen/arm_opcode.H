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
// MACHINE GENERATED -- DO NOT EDIT!!!
//

#ifdef ARM_OPCODE_H
#error duplicate inclusion of ARM_OPOCODE_H
#else
#define ARM_OPCODE_H

/*! @defgroup INS_BASIC_API_ARM
  Opcode enumeration for arm
*/

typedef enum
{
    ARM_OPCODE_INVALID,
    ARM_OPCODE_MUL,
    ARM_OPCODE_MULA,
    ARM_OPCODE_UMULL,
    ARM_OPCODE_SMULL,
    ARM_OPCODE_UMLAL,
    ARM_OPCODE_SMLAL,
    ARM_OPCODE_LDR_2,
    ARM_OPCODE_STR_2,
    ARM_OPCODE_LDRS_1,
    ARM_OPCODE_LDRS_2,
    ARM_OPCODE_AND,
    ARM_OPCODE_EOR,
    ARM_OPCODE_SUB,
    ARM_OPCODE_RSB,
    ARM_OPCODE_ADD,
    ARM_OPCODE_ADC,
    ARM_OPCODE_SBC,
    ARM_OPCODE_RSC,
    ARM_OPCODE_SWP_4,
    ARM_OPCODE_SWP_1,
    ARM_OPCODE_JX,
    ARM_OPCODE_JLX,
    ARM_OPCODE_CLZ,
    ARM_OPCODE_MTCSR,
    ARM_OPCODE_MTSSR,
    ARM_OPCODE_MTCSR_C,
    ARM_OPCODE_MTSSR_C,
    ARM_OPCODE_MTCSR_X,
    ARM_OPCODE_MTSSR_X,
    ARM_OPCODE_MTCSR_CX,
    ARM_OPCODE_MTSSR_CX,
    ARM_OPCODE_MTCSR_S,
    ARM_OPCODE_MTSSR_S,
    ARM_OPCODE_MTCSR_CS,
    ARM_OPCODE_MTSSR_CS,
    ARM_OPCODE_MTCSR_XS,
    ARM_OPCODE_MTSSR_XS,
    ARM_OPCODE_MTCSR_CXS,
    ARM_OPCODE_MTSSR_CXS,
    ARM_OPCODE_MTCSR_F,
    ARM_OPCODE_MTSSR_F,
    ARM_OPCODE_MTCSR_CF,
    ARM_OPCODE_MTSSR_CF,
    ARM_OPCODE_MTCSR_XF,
    ARM_OPCODE_MTSSR_XF,
    ARM_OPCODE_MTCSR_CXF,
    ARM_OPCODE_MTSSR_CXF,
    ARM_OPCODE_MTCSR_SF,
    ARM_OPCODE_MTSSR_SF,
    ARM_OPCODE_MTCSR_CSF,
    ARM_OPCODE_MTSSR_CSF,
    ARM_OPCODE_MTCSR_XSF,
    ARM_OPCODE_MTSSR_XSF,
    ARM_OPCODE_MTCSR_CXSF,
    ARM_OPCODE_MTSSR_CXSF,
    ARM_OPCODE_MFCSR,
    ARM_OPCODE_MFSSR,
    ARM_OPCODE_TST,
    ARM_OPCODE_TEQ,
    ARM_OPCODE_CMP,
    ARM_OPCODE_CMN,
    ARM_OPCODE_ORR,
    ARM_OPCODE_MOV,
    ARM_OPCODE_BIC,
    ARM_OPCODE_MVN,
    ARM_OPCODE_LDR_4,
    ARM_OPCODE_LDP,
    ARM_OPCODE_LDR_1,
    ARM_OPCODE_STR_4,
    ARM_OPCODE_STR_1,
    ARM_OPCODE_BLX,
    ARM_OPCODE_STM,
    ARM_OPCODE_STM_U,
    ARM_OPCODE_LDM,
    ARM_OPCODE_LDM_U,
    ARM_OPCODE_B,
    ARM_OPCODE_BL,
    ARM_OPCODE_FSMAC,
    ARM_OPCODE_FSNMAC,
    ARM_OPCODE_FSMSC,
    ARM_OPCODE_FSNMSC,
    ARM_OPCODE_FSMUL,
    ARM_OPCODE_FSNMUL,
    ARM_OPCODE_FSADD,
    ARM_OPCODE_FSSUB,
    ARM_OPCODE_FSDIV,
    ARM_OPCODE_FSCPY,
    ARM_OPCODE_FSABS,
    ARM_OPCODE_FSNEG,
    ARM_OPCODE_FSSQRT,
    ARM_OPCODE_FDMAC,
    ARM_OPCODE_FDNMAC,
    ARM_OPCODE_FDMSC,
    ARM_OPCODE_FDNMSC,
    ARM_OPCODE_FDMUL,
    ARM_OPCODE_FDNMUL,
    ARM_OPCODE_FDADD,
    ARM_OPCODE_FDSUB,
    ARM_OPCODE_FDDIV,
    ARM_OPCODE_FDCPY,
    ARM_OPCODE_FDABS,
    ARM_OPCODE_FDNEG,
    ARM_OPCODE_FDSQRT,
    ARM_OPCODE_LDFS,
    ARM_OPCODE_LDFD,
    ARM_OPCODE_LDFE,
    ARM_OPCODE_LDFP,
    ARM_OPCODE_STFS,
    ARM_OPCODE_STFD,
    ARM_OPCODE_STFE,
    ARM_OPCODE_STFP,
    ARM_OPCODE_LFM,
    ARM_OPCODE_SFM,
    ARM_OPCODE_MVFS,
    ARM_OPCODE_MNFS,
    ARM_OPCODE_ABSS,
    ARM_OPCODE_RNDS,
    ARM_OPCODE_SQTS,
    ARM_OPCODE_LOGS,
    ARM_OPCODE_LGNS,
    ARM_OPCODE_EXPS,
    ARM_OPCODE_SINS,
    ARM_OPCODE_COSS,
    ARM_OPCODE_TANS,
    ARM_OPCODE_ASNS,
    ARM_OPCODE_ACSS,
    ARM_OPCODE_ATNS,
    ARM_OPCODE_URDS,
    ARM_OPCODE_NRMS,
    ARM_OPCODE_ADFS,
    ARM_OPCODE_MUFS,
    ARM_OPCODE_SUFS,
    ARM_OPCODE_RSFS,
    ARM_OPCODE_DVFS,
    ARM_OPCODE_RDFS,
    ARM_OPCODE_POWS,
    ARM_OPCODE_RPWS,
    ARM_OPCODE_RMFS,
    ARM_OPCODE_FMLS,
    ARM_OPCODE_FDVS,
    ARM_OPCODE_FRDS,
    ARM_OPCODE_POLS,
    ARM_OPCODE_MVFD,
    ARM_OPCODE_MNFD,
    ARM_OPCODE_ABSD,
    ARM_OPCODE_RNDD,
    ARM_OPCODE_SQTD,
    ARM_OPCODE_LOGD,
    ARM_OPCODE_LGND,
    ARM_OPCODE_EXPD,
    ARM_OPCODE_SIND,
    ARM_OPCODE_COSD,
    ARM_OPCODE_TAND,
    ARM_OPCODE_ASND,
    ARM_OPCODE_ACSD,
    ARM_OPCODE_ATND,
    ARM_OPCODE_URDD,
    ARM_OPCODE_NRMD,
    ARM_OPCODE_ADFD,
    ARM_OPCODE_MUFD,
    ARM_OPCODE_SUFD,
    ARM_OPCODE_RSFD,
    ARM_OPCODE_DVFD,
    ARM_OPCODE_RDFD,
    ARM_OPCODE_POWD,
    ARM_OPCODE_RPWD,
    ARM_OPCODE_RMFD,
    ARM_OPCODE_FMLD,
    ARM_OPCODE_FDVD,
    ARM_OPCODE_FRDD,
    ARM_OPCODE_POLD,
    ARM_OPCODE_MVFE,
    ARM_OPCODE_MNFE,
    ARM_OPCODE_ABSE,
    ARM_OPCODE_RNDE,
    ARM_OPCODE_SQTE,
    ARM_OPCODE_LOGE,
    ARM_OPCODE_LGNE,
    ARM_OPCODE_EXPE,
    ARM_OPCODE_SINE,
    ARM_OPCODE_COSE,
    ARM_OPCODE_TANE,
    ARM_OPCODE_ASNE,
    ARM_OPCODE_ACSE,
    ARM_OPCODE_ATNE,
    ARM_OPCODE_URDE,
    ARM_OPCODE_NRME,
    ARM_OPCODE_ADFE,
    ARM_OPCODE_MUFE,
    ARM_OPCODE_SUFE,
    ARM_OPCODE_RSFE,
    ARM_OPCODE_DVFE,
    ARM_OPCODE_RDFE,
    ARM_OPCODE_POWE,
    ARM_OPCODE_RPWE,
    ARM_OPCODE_RMFE,
    ARM_OPCODE_FMLE,
    ARM_OPCODE_FDVE,
    ARM_OPCODE_FRDE,
    ARM_OPCODE_POLE,
    ARM_OPCODE_FLTS,
    ARM_OPCODE_FLTD,
    ARM_OPCODE_FLTE,
    ARM_OPCODE_FIX,
    ARM_OPCODE_WFS,
    ARM_OPCODE_RFS,
    ARM_OPCODE_WFC,
    ARM_OPCODE_RFC,
    ARM_OPCODE_CMF,
    ARM_OPCODE_CNF,
    ARM_OPCODE_CMFE,
    ARM_OPCODE_CNFE,
    ARM_OPCODE_SWI,
    ARM_OPCODE_CONST_LDR_4,
    ARM_OPCODE_CONST_LDFD,
    ARM_OPCODE_CONST_LDFS,
    ARM_OPCODE_CONST_CMP,
    ARM_OPCODE_LAST
}ARM_OPCODE;

#endif
