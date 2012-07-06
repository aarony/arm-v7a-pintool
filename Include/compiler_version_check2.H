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
/*
  @ORIGINAL_AUTHOR: Robert Muth
  This header file ensures that pin and the pintool will be compiled with compatible
  compilers.
  It relies on the generated file cc_used_XXX.H
*/

#if defined(TARGET_IA32)

# if defined(TARGET_WINDOWS)
#  if defined(TARGET_IA32)
#   include "cc_used_ia32_w.CVH"
#  else
#   error "Need IA32 or EM64T on windows"
#  endif
# elif defined(TARGET_LINUX)
#  include "cc_used_ia32_l.CVH"
# elif defined(TARGET_BSD)
#  include "cc_used_ia32_b.CVH"
# elif defined(TARGET_MAC)
#  include "cc_used_ia32_m.CVH"
# else
#  error "bad target os for IA32"
# endif

#elif defined(TARGET_IA32E)

# if defined(TARGET_WINDOWS)
#   include "cc_used_ia32e_w.CVH"
# elif defined(TARGET_BSD)
#  include "cc_used_ia32e_b.CVH"
# elif defined(TARGET_LINUX)
#  include "cc_used_ia32e_l.CVH"
# else
#  error "bad target os for EM64T"
#endif

#elif defined(TARGET_ARM)
#include "cc_used_arm_l.CVH"

#elif defined(TARGET_IPF)
#include "cc_used_ipf_l.CVH"
#else
#error "bad target"
#endif

#if defined(_MSC_VER)
#elif defined(__INTEL_COMPILER)
#elif defined(__GNUC__)
#else
#error "pin must be compiled with Microsoft CL, Intel ICC/ICL  or with gcc/g++"
#endif

#if defined(_MSC_VER)
/* FIXME skipping compiler checks for now */
#else

#if !defined(CC_USED__) || !defined(CC_USED_MINOR__) || !defined(CC_USED_PATCHLEVEL__)
#error "internal error pin compiler configuration not available "
#endif

#if CC_USED__ != __GNUC__
/*#error "pin compiler and pin tool compiler differ in major version number"*/
#endif

#if CC_USED__ == 3 && CC_USED_MINOR__ == 2 && __GNUC__ == 3 && __GNUC_MINOR__ == 3

// The only exception is 3.2library is ok with 3.3tool

#else

#if CC_USED_MINOR__ != __GNUC_MINOR__
/*#error "pin compiler and pin tool compiler differ in minor version number. See cc_used_*.CVH for kit compiler version."*/
#endif

// We don't think patch level matters for BSD
#if __GNUC_PATCHLEVEL__ < CC_USED_PATCHLEVEL__ && !defined(TARGET_BSD)
#error "pin compiler has higher patch level than pin tool compiler"
#endif
#endif
#endif

