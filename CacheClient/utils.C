/*BEGIN_LEGAL 
INTEL CONFIDENTIAL
Copyright 2002-2005 Intel Corporation All Rights Reserved.

The source code contained or described herein and all documents
related to the source code (Material) are owned by Intel Corporation
or its suppliers or licensors. Title to the Material remains with
Intel Corporation or its suppliers and licensors. The Material may
contain trade secrets and proprietary and confidential information of
Intel Corporation and its suppliers and licensors, and is protected by
worldwide copyright and trade secret laws and treaty provisions. No
part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in
any way without Intels prior express written permission.  No license
under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or
delivery of the Materials, either expressly, by implication,
inducement, estoppel or otherwise. Any license under such intellectual
property rights must be express and approved by Intel in writing.

Unless otherwise agreed by Intel in writing, you may not remove or
alter this notice or any other notice embedded in Materials by Intel
or Intels suppliers or licensors in any way.
END_LEGAL */
//
//  ORIGINAL_AUTHOR: Kim Hazelwood
//
//  This is a utility file that contains useful routines for 
//      all cache client tools

#include "pin.H"
#include "utils.H"
#include <iostream>

using namespace std;

/* ================================================================== */
/*
  Convert an unsigned integer (representing bytes) into a string 
  that uses KB or MB as necessary
*/
string BytesToString(UINT32 numBytes)
{
    if (numBytes < 10240)
        return decstr(numBytes) + " bytes"; 
    else if (numBytes < (1024*10240))
        return decstr(numBytes>>10) + " KB"; 
    else 
        return decstr(numBytes>>20) + " MB"; 
}

/* ================================================================== */
/*
  Print details of a cache initialization
*/
VOID PrintInitInfo()
{
    cout << "Cache Initialization Complete\t";

    UINT32 block_size = CODECACHE_BlockSize();
    UINT32 cache_limit = CODECACHE_CacheSizeLimit();
    
    if (cache_limit)
        cout << "[cache_limit=" << BytesToString(cache_limit) ;
    else 
        cout << "[cache_limit=unlimited" ;
    
    cout << ", cache_block=" << BytesToString(block_size) << "]" << endl;
}

/* ================================================================== */
/*
  Print command-line switches on error
*/
INT32 Usage()
{
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}
