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
/* ================================================================== */
/* Miscellaneous Functions (possibily used by multiple pin tools)     */
/* ================================================================== */

#ifndef PINTOOL_UTILITIES_H
#define PINTOOL_UTILITIES_H

#include "pin.H"
#include "portability.H"
#include <iostream>
#include <fstream>


/* ------------------------------------------------------------------ */
/* global constants                                                   */
/* ------------------------------------------------------------------ */

ADDRINT STACKLIMIThi = 0xbfffffff;

/* ------------------------------------------------------------------ */
/* PrintHexWord:                                                      */
/* prints a formatted version of a little endian memory word          */
/* ------------------------------------------------------------------ */

VOID PrintHexWord(ADDRINT addr, fstream& outfile) 
{
    for (INT32 offset = sizeof(ADDRINT) - 1; offset >= 0; offset--) 
    {
        UINT32 val = (UINT32)*(UINT8*)(addr + offset);
        if (val < 0xf)        // formatting: pad single digits
        {
            outfile << "0" << flush;
        }
        outfile << hex << val << flush;
    }
}

/* ------------------------------------------------------------------ */
/* GetArg (overloaded):                                               */
/* extracts the integer argument value or flag from the command line  */
/* ------------------------------------------------------------------ */

VOID GetArg(UINT32 argc, char** argv, const char* argname, UINT32& arg, UINT32 default_val) 
{
    BOOL found = false;
    UINT32 i = 0;
    do 
    {
        string* tmp = new string(argv[i]);
        if (tmp->find(argname) != string::npos) 
        {
            ASSERTX((i + 1) < argc);
            arg = atoi(argv[i + 1]);
            found = true;
        }
        else 
        {
            i++;
        }
        delete tmp;
    } while (!found && (i < argc));
    if (!found) 
    {
        arg = default_val;
    }
}

VOID GetArg(UINT32 argc, char** argv, const char* argname, BOOL& arg) 
{
    BOOL found = false;
    UINT32 i = 0;
    do 
    {
        string* tmp = new string(argv[i]);
        if (tmp->find(argname) != string::npos) 
        {
            ASSERTX((i + 1) < argc);
            arg = true;
            found = true;
        }
        else 
        {
            i++;
        }
        delete tmp;
    } while (!found && (i < argc));
    if (!found) 
    {
        arg = false;
    }
}
    
/* ------------------------------------------------------------------ */

#endif  // #ifndef PINTOOL_UTILITIES_H
