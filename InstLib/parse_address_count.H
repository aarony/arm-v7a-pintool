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
#ifndef _PARSE_ADDRESS_COUNT_H_
# define _PARSE_ADDRESS_COUNT_H_
#include "common.h"
namespace INSTLIB 
{

class ADDRESS_COUNT
{
  public:
    ADDRESS_COUNT()
        : name(""),
          address(0),
          count(0),
          offset(0),
          rearm(false),
          always_enabled(false)
    {}

    void print()
    {
        cerr << "name: " << name
             << " address: " << hex << address << dec
             << " offset: " << hex << offset << dec
             << " rearm: " << (rearm?"true":"false")
             << " always_enabled: " << (always_enabled?"true":"false")
             << endl;
    }
    
    string name; 
    ADDRINT address;
    UINT64 count; 
    UINT64 offset;
    BOOL rearm;
    BOOL always_enabled; // ignores count

    
};

LOCALFUN unsigned int
split_args(const string& sep, 
           const string& input, 
           vector<string>& output_array)
{
    // returns the number of args
    // rip off the separator characters and split the src string based on separators.
    
    // find the string between last_pos and pos. pos is after last_pos
    string::size_type last_pos = input.find_first_not_of(sep, 0);
    string::size_type pos = input.find_first_of(sep, last_pos);  
    //XTMSG("input " << input << " \tlast_pos " << last_pos << " pos " << pos);
    int i=0;
    while( pos != string::npos && last_pos != string::npos ) 
    {
        string a = input.substr(last_pos, pos-last_pos);
        output_array.push_back(a); 
        //XTMSG("\t\tlast_pos " << last_pos << " pos " << pos << " i " << i);
        last_pos = input.find_first_not_of(sep, pos);
        pos = input.find_first_of(sep, last_pos);  
        i++;
    }
    if (last_pos != string::npos && pos == string::npos)
    {
        //XTMSG("\t\tGetting last substring at " << last_pos);
        string a = input.substr(last_pos); // get the rest of the string
        output_array.push_back(a);
        i++;
    }
    //XTMSG("\t returning " << i);
    return i;
}


LOCALFUN ADDRESS_COUNT ParseAddressCount(string str)
{
    ADDRESS_COUNT retval;
    string addressStr;
    string countStr = "1";
    BOOL foundCount = false;
    string offsetStr;


    vector<string> tokens;
    const string sep = ":";
    unsigned int args = split_args(sep, str, tokens);
    ASSERTX(args != 0);
    addressStr = tokens[0];
    for(unsigned int i = 1 ;i < args ;i++)
    {
        const unsigned int length = tokens[i].size();
        if (length > 0 && isdigit(tokens[i][0]))
        {
            foundCount = true;
            countStr = tokens[i];
        }
        else if (tokens[i] == "repeat" || tokens[i] == "rearm")
        {
            retval.rearm = true;
        }
        else
        {
            cerr << "Unrecognized token in controller argument: " << tokens[i]
                 << endl;
            ASSERTX(0);
        }
    }
  
    if (retval.rearm && foundCount == false)
    {
        retval.always_enabled = true;
    }
    retval.count = Uint64FromString(countStr);

    if (isdigit(addressStr[0]))
    {
        retval.address = AddrintFromString(addressStr);
        retval.name = "";
    }
    else
    {
        retval.address = 0;
        // Split at +
        UINT64 pos = addressStr.find("+");
     
        if (pos == string::npos)
        {
            retval.name = addressStr;
            retval.offset = 0;
        }
        else
        {
            retval.name = addressStr.substr(0,pos);
            offsetStr = addressStr.substr(pos+1,addressStr.length());
            retval.offset = Uint64FromString(offsetStr);
        }
    }

    return retval;
}



} // namespace
#endif
