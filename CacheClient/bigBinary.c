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
// Developed by Robert Cohn
// This file produces a large binary that can be used 
//   for testing purposes
// Sample use: pin -t <pintool> -- bigBinary

#define FUNS(name)     \
    name()        \
    {                    \
        int i;           \
        for (i = 0; i < n; i++)                 \
        {                                       \
            a[i] = i + 2;                       \
        }                                       \
    }                                           \

#define FUNS10(number)  \
    FUNS(number##0)     \
    FUNS(number##1)     \
    FUNS(number##2)     \
    FUNS(number##3)     \
    FUNS(number##4)     \
    FUNS(number##5)     \
    FUNS(number##6)     \
    FUNS(number##7)     \
    FUNS(number##8)     \
    FUNS(number##9)

#define FUNS100(number)  \
    FUNS10(number##0)     \
    FUNS10(number##1)     \
    FUNS10(number##2)     \
    FUNS10(number##3)     \
    FUNS10(number##4)     \
    FUNS10(number##5)     \
    FUNS10(number##6)     \
    FUNS10(number##7)     \
    FUNS10(number##8)     \
    FUNS10(number##9)

#define FUNS1000(number)  \
    FUNS100(number##0)     \
    FUNS100(number##1)     \
    FUNS100(number##2)     \
    FUNS100(number##3)     \
    FUNS100(number##4)     \
    FUNS100(number##5)     \
    FUNS100(number##6)     \
    FUNS100(number##7)     \
    FUNS100(number##8)     \
    FUNS100(number##9)


#define CALLFUNS(name)      \
    name();

#define CALLFUNS10(name)   \
    CALLFUNS(name##0)        \
    CALLFUNS(name##1)        \
    CALLFUNS(name##2)        \
    CALLFUNS(name##3)        \
    CALLFUNS(name##4)        \
    CALLFUNS(name##5)        \
    CALLFUNS(name##6)        \
    CALLFUNS(name##7)        \
    CALLFUNS(name##8)        \
    CALLFUNS(name##9)
    
#define CALLFUNS100(name)   \
    CALLFUNS10(name##0)        \
    CALLFUNS10(name##1)        \
    CALLFUNS10(name##2)        \
    CALLFUNS10(name##3)        \
    CALLFUNS10(name##4)        \
    CALLFUNS10(name##5)        \
    CALLFUNS10(name##6)        \
    CALLFUNS10(name##7)        \
    CALLFUNS10(name##8)        \
    CALLFUNS10(name##9)
    
int n = 10;
int a[100];

FUNS100(foo)
    
int main()
{
    CALLFUNS100(foo)
    return 0;
}
