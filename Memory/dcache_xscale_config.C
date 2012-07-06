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
// @ORIGINAL_AUTHOR: Artur Klauser
//

/*! @file
 *  This file contains an ISA-portable PIN tool for functional simulation of
 *  the XScale L1 data cache
 */

#include <iostream>

#include "pin.H"

typedef UINT64 CACHE_STATS; // type of cache hit/miss counters

#include "pin_cache.H"


KNOB<UINT32>   KnobCacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "-s1", "32768", "l1 cache size in bytes");

KNOB<UINT32>   KnobCacheAssociativity(KNOB_MODE_WRITEONCE, "pintool",
    "-a1", "32", "l1 cache associativity");

KNOB<UINT32>   KnobCacheLineSize(KNOB_MODE_WRITEONCE, "pintool",
    "-l1", "32", "l1 cache size in bytes");

namespace DL1
{

    const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_NO_ALLOCATE;

    const UINT32 max_sets = 128;

    const UINT32 max_associativity = 32;


    typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
};

LOCALVAR DL1::CACHE *dl1;


LOCALFUN VOID Fini(int code, VOID * v)
{
    std::cerr << *dl1;
}

LOCALFUN VOID MemRefSingle(CACHE_BASE::ACCESS_TYPE accessType, ADDRINT addr)
{
    // first level D-cache: single cache-line access
    dl1->AccessSingleLine(addr, accessType);
}

LOCALFUN VOID MemRefMulti(CACHE_BASE::ACCESS_TYPE accessType, ADDRINT addr, UINT32 size)
{
    // first level D-cache: potentially multiple cache-line access
    dl1->Access(addr, size, accessType);
}

LOCALFUN VOID Instruction(INS ins, void * v)
{
    if (INS_IsMemoryRead(ins))
    {
        const UINT32 size = INS_MemoryReadSize(ins);
        // we assume accesses <= 4 bytes stay in the same cache line
        // to speed up cache access lookups
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_LOAD,
            IARG_MEMORYREAD_EA,
            IARG_MEMORYREAD_SIZE,
            IARG_END);
    }

    if (INS_IsMemoryWrite(ins))
    {
        const UINT32 size = INS_MemoryWriteSize(ins);
        const AFUNPTR countFun = (size <= 4 ? (AFUNPTR) MemRefSingle : (AFUNPTR) MemRefMulti);

        // only predicated-on memory instructions access D-cache
        INS_InsertPredicatedCall(
            ins, IPOINT_BEFORE, countFun,
            IARG_UINT32, CACHE_BASE::ACCESS_TYPE_STORE,
            IARG_MEMORYWRITE_EA,
            IARG_MEMORYWRITE_SIZE,
            IARG_END);
    }
}

GLOBALFUN int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    const UINT32 size = KnobCacheSize.Value();
    const UINT32 linesize = KnobCacheLineSize.Value();
    const UINT32 associativity = KnobCacheAssociativity.Value();

    ASSERTX(  associativity <= DL1::max_associativity );
    ASSERTX( size /(associativity*linesize )<= DL1::max_sets );
        
    // create the cache object
    dl1 = new DL1::CACHE("L1 Data Cache", size, linesize, associativity);

    // Never returns
    PIN_StartProgram();

    return 0; // make compiler happy
}
