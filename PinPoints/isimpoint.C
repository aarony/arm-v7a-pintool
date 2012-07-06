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
/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Robert Muth
*/

/* ===================================================================== */
/*! @file
 *  This file contains an ISA-portable PIN tool for counting dynamic instructions
 */

#include <map>
#include <iostream>
#include <fstream>
#include <string.h>

#include "pin.H"
#include "instlib.H"
#include "time_warp.H"

#define MAX_THREADS 8
#define MAX_IMAGES 250

using namespace INSTLIB;
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
TIME_WARP tw;
#endif

class IMG_INFO
{
    public:
        IMG_INFO(IMG img);
        INT32 Id() { return _imgId;}
        CHAR * Name() { return _name;}
        ADDRINT  LowAddress() { return _low_address;}
        static INT32 _currentImgId;
    private:
        CHAR * _name; 
        ADDRINT _low_address; 
        // static members
        INT32 _imgId;
};

IMG_INFO * img_info[MAX_IMAGES]; 

IMG_INFO::IMG_INFO(IMG img)
    : _imgId(_currentImgId++)
{
    _name = (CHAR *) calloc(strlen(IMG_Name(img).c_str())+1, 1);
    strcpy(_name,IMG_Name(img).c_str());
    _low_address = IMG_LowAddress(img);
}

UINT32 FindImgInfoId(IMG img)
{
    if (!IMG_Valid(img))
        return 0;
    
    ADDRINT low_address = IMG_LowAddress(img);
    
    for (UINT32 i = IMG_INFO::_currentImgId-1; i >=1; i--)
    {
        if(img_info[i]->LowAddress() == low_address)
            return i;
    }
    // cerr << "FindImgInfoId(0x" << hex << low_address << ")" <<   endl;
    return 0;
}

class BLOCK_KEY
{
    friend BOOL operator<(const BLOCK_KEY & p1, const BLOCK_KEY & p2);
        
  public:
    BLOCK_KEY(ADDRINT s, ADDRINT e, USIZE z) : _start(s),_end(e),_size(z) {};
    BOOL IsPoint() const { return (_start - _end) == 1;  }
    ADDRINT Start() const { return _start; }
    ADDRINT End() const { return _end; }
    USIZE Size() const { return _size; }
    BOOL Contains(ADDRINT addr) const;
    
  private:
    const ADDRINT _start;
    const ADDRINT _end;
    const USIZE _size;
};

class BLOCK
{
  public:
    BLOCK(const BLOCK_KEY & key, INT32 instructionCount, IMG i);
    INT32 StaticInstructionCount() const { return _staticInstructionCount; }
    VOID Execute(INT32 tid) { _sliceBlockCount[tid]++; }
    VOID EmitSliceEnd(INT32 tid) ;
    VOID EmitProgramEnd(const BLOCK_KEY & key, INT32 tid) const;
    INT64 GlobalBlockCount(INT32 tid) const { return _globalBlockCount[tid] + _sliceBlockCount[tid]; }
    UINT32 ImgId() const { return _imgId; }
    const BLOCK_KEY & Key() const { return _key; }
    
    
  private:
    INT32 Id() const { return _id; }
    INT32 SliceInstructionCount(INT32 tid) const { return _sliceBlockCount[tid] * _staticInstructionCount; }

    // static members
    static INT32 _currentId;

    const INT32 _staticInstructionCount;
    const INT32 _id;
    const BLOCK_KEY _key;

    INT32 _sliceBlockCount[MAX_THREADS];
    INT64 _globalBlockCount[MAX_THREADS];
    UINT32 _imgId;
};


LOCALTYPE typedef pair<BLOCK_KEY, BLOCK*> BLOCK_PAIR;
LOCALTYPE typedef multimap<BLOCK_KEY, BLOCK*> BLOCK_MAP;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

LOCALVAR BLOCK_MAP block_map;
LOCALVAR string commandLine;

class PROFILE
{
    public: 
    PROFILE()
    {
        first = true;
        GlobalInstructionCount = 0;
        SliceTimer = 0;
    }
    ofstream BbFile;
    INT64 GlobalInstructionCount;
    // The first time, we want a marker, but no T vector
    BOOL first;
    // Emit the first marker immediately
    INT32 SliceTimer;
};

// LOCALVAR INT32 maxThread = 0;
LOCALVAR PROFILE ** profiles;
// LOCALVAR INT32 firstPid = 0;


INT32 BLOCK::_currentId = 1;
INT32 IMG_INFO::_currentImgId = 1;


/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "out", "specify bb file name");
KNOB<INT32>  KnobSliceSize(KNOB_MODE_WRITEONCE,  "pintool",
    "slice_size", "100000000", "slice size in instructions");
KNOB<BOOL>  KnobNoSymbolic(KNOB_MODE_WRITEONCE,  "pintool",
    "nosymbolic", "0", "Do not emit symbolic information for markers");


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This tool collects profiles for SimPoint.\n"
        "\n";


    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

VOID BLOCK::EmitSliceEnd(INT32 tid)
{
    if (_sliceBlockCount[tid] == 0)
        return;
    
    profiles[tid]->BbFile << ":" << dec << Id() << ":" << dec << SliceInstructionCount(tid) << " ";
    _globalBlockCount[tid] += _sliceBlockCount[tid];
    _sliceBlockCount[tid] = 0;
}


BOOL operator<(const BLOCK_KEY & p1, const BLOCK_KEY & p2)
{
    if (p1.IsPoint())
        return p1._start < p2._start;

    if (p2.IsPoint())
        return p1._end <= p2._start;
    
    if (p1._start == p2._start)
        return p1._end < p2._end;
    
    return p1._start < p2._start;
}

BOOL BLOCK_KEY::Contains(ADDRINT address) const
{
    if (address >= _start && address <= _end)
        return true;
    else
        return false;
}

/* ===================================================================== */

LOCALFUN VOID EmitSliceEnd(ADDRINT endMarker, UINT32 imgId, INT32 tid)
{
    
    INT64 markerCount = 0;
    
    profiles[tid]->BbFile << "# Slice at " << dec << profiles[tid]->GlobalInstructionCount << endl;
    
    if (!profiles[tid]->first)
        profiles[tid]->BbFile << "T" ;
    else
    {
    // Input merging will change the name of the input
        profiles[tid]->BbFile << "I: 0" << endl;
        profiles[tid]->BbFile << "P: " << dec << tid << endl;
        profiles[tid]->BbFile << "C: sum:dummy Command:" << commandLine << endl;
    }

    for (BLOCK_MAP::iterator bi = block_map.begin(); bi != block_map.end(); bi++)
    {
        BLOCK * block = bi->second;
        const BLOCK_KEY & key = bi->first;

        if (key.Contains(endMarker))
        {
            markerCount += block->GlobalBlockCount(tid);
        }
        
        if (!profiles[tid]->first)
            block->EmitSliceEnd(tid);
    }

    if (!profiles[tid]->first)
        profiles[tid]->BbFile << endl;

    profiles[tid]->first = false;
    
    if (KnobNoSymbolic)
    {
        profiles[tid]->BbFile << "M: " << hex << endMarker << " " << dec << markerCount << endl;
    }
    else
    {
        if(!imgId)
        {
            profiles[tid]->BbFile << "S: " << hex << endMarker << " " << dec << markerCount << " " << "no_image" << " " << hex  << 0 << endl;
        }
        else
        {
            profiles[tid]->BbFile << "S: " << hex << endMarker << " " << dec << markerCount << " " << img_info[imgId]->Name() << " " << hex  <<img_info[imgId]->LowAddress() << " + " << hex << endMarker-img_info[imgId]->LowAddress() << endl;
        }
    }
}

#if defined(TARGET_ARM)
VOID CountBlock(BLOCK * block, INT32 tid)
{
    block->Execute(tid);

    profiles[tid]->SliceTimer -= block->StaticInstructionCount();
    profiles[tid]->GlobalInstructionCount += block->StaticInstructionCount();

    if (profiles[tid]->SliceTimer < 0)
    {
        EmitSliceEnd(block->Key().End(), block->ImgId(), tid);
        profiles[tid]->SliceTimer = KnobSliceSize;
    }
}
#else
int CountBlock_If(BLOCK * block, INT32 tid)
{
    block->Execute(tid);

    profiles[tid]->SliceTimer -= block->StaticInstructionCount();

    return(profiles[tid]->SliceTimer < 0);
}

VOID CountBlock_Then(BLOCK * block, INT32 tid)
{
    profiles[tid]->GlobalInstructionCount += (KnobSliceSize - profiles[tid]->SliceTimer);
    EmitSliceEnd(block->Key().End(), block->ImgId(), tid);
    profiles[tid]->SliceTimer = KnobSliceSize;
}
#endif

BLOCK::BLOCK(const BLOCK_KEY & key, INT32 instructionCount, IMG img)
    :
    _staticInstructionCount(instructionCount),
    _id(_currentId++),
    _key(key)
{
    _imgId = FindImgInfoId(img);
    for (INT32 tid = 0; tid < MAX_THREADS; tid++)
    {
        _sliceBlockCount[tid] = 0;
        _globalBlockCount[tid] = 0;
    }
}

LOCALFUN BLOCK * LookupBlock(BBL bbl)
{
    BLOCK_KEY key(INS_Address(BBL_InsHead(bbl)), INS_Address(BBL_InsTail(bbl)), BBL_Size(bbl));
    BLOCK_MAP::const_iterator bi = block_map.find(key);
    
    if (bi == block_map.end())
    {
        // Block not there, add it
        RTN rtn = INS_Rtn(BBL_InsHead(bbl));
        SEC sec = SEC_Invalid();
        IMG img = IMG_Invalid();
        if(RTN_Valid(rtn))
            sec = RTN_Sec(rtn);
        if(SEC_Valid(sec))
            img = SEC_Img(sec);
        BLOCK * block = new BLOCK(key, BBL_NumIns(bbl), img);
        block_map.insert(BLOCK_PAIR(key, block));

        return block;
    }
    else
    {
        return bi->second;
    }
}

/* ===================================================================== */

VOID Trace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        BLOCK * block = LookupBlock(bbl);
#if defined(TARGET_ARM)
        INS_InsertCall(BBL_InsTail(bbl), IPOINT_BEFORE, (AFUNPTR)CountBlock, IARG_PTR, block, IARG_THREAD_ID, IARG_END);
#else
        INS_InsertIfCall(BBL_InsTail(bbl), IPOINT_BEFORE, (AFUNPTR)CountBlock_If, IARG_PTR, block, IARG_THREAD_ID, IARG_END);
        INS_InsertThenCall(BBL_InsTail(bbl), IPOINT_BEFORE, (AFUNPTR)CountBlock_Then, IARG_PTR, block, IARG_THREAD_ID, IARG_END);
#endif
    }
}

/* ===================================================================== */

VOID Image(IMG img, VOID * v)
{
    ASSERTX(IMG_INFO::_currentImgId < (MAX_IMAGES - 1));
    img_info[IMG_INFO::_currentImgId] = new IMG_INFO(img); 
    profiles[0]->BbFile << "G: " << IMG_Name(img) << " LowAddress: " << hex  << IMG_LowAddress(img) << " LoadOffset: " << hex << IMG_LoadOffset(img) << endl;
}


VOID BLOCK::EmitProgramEnd(const BLOCK_KEY & key, INT32 tid) const
{
    if (_globalBlockCount[tid] == 0)
        return;
    
    profiles[tid]->BbFile << "Block id: " << dec << _id << " " << hex << key.Start() << ":" << key.End() << dec
           << " static instructions: " << _staticInstructionCount
           << " block count: " << _globalBlockCount[tid]
           << " block size: " << key.Size()
           << endl;
}

LOCALFUN VOID EmitProgramEnd(INT32 tid)
{
    profiles[tid]->BbFile << "Dynamic instruction count " << dec << profiles[tid]->GlobalInstructionCount << endl;
    profiles[tid]->BbFile << "SliceSize: " << dec << KnobSliceSize << endl;
    for (BLOCK_MAP::const_iterator bi = block_map.begin(); bi != block_map.end(); bi++)
    {
        bi->second->EmitProgramEnd(bi->first, tid);
    }
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    for (INT32 tid = 0; tid < MAX_THREADS; tid++)
    {
        EmitProgramEnd(tid);
        profiles[tid]->BbFile << "End of bb" << endl;
        profiles[tid]->BbFile.close();
    }
}

VOID ThreadBegin(UINT32 tid, VOID * sp, int flags, VOID *v)
{
    char num[100];
    sprintf(num, ".T.%d.bb", tid);
    string tname = num;
    ASSERTX(tid < MAX_THREADS);
    profiles[tid]->BbFile.open((KnobOutputFile.Value()+tname).c_str());
    profiles[tid]->BbFile.setf(ios::showbase);
}

VOID ThreadEnd(UINT32 threadid, INT32 code, VOID *v)
{
}


VOID GetCommand(int argc, char *argv[])
{
    for (INT32 i = 0; i < argc; i++)
    {
            commandLine += " ";
            commandLine += argv[i];
    }
}
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    GetCommand(argc, argv);

    //maxThread = MaxThreadsKnob.ValueInt64();
    profiles = new PROFILE*[MAX_THREADS];
    memset(profiles, 0, MAX_THREADS * sizeof(profiles[0]));

    PIN_AddThreadBeginFunction(ThreadBegin, 0);
    PIN_AddThreadEndFunction(ThreadEnd, 0);


    for (INT32 tid = 0; tid < MAX_THREADS; tid++)
    {
        profiles[tid] = new PROFILE();
    }
    profiles[0]->BbFile.open((KnobOutputFile.Value()+".T.0.bb").c_str());
    profiles[0]->BbFile.setf(ios::showbase);

#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    tw.CheckKnobs(0);
#endif
#if defined(TARGET_MAC)
    // On Mac, ImageLoad() works only after we call PIN_InitSymbols().
    PIN_InitSymbols();
#endif

    TRACE_AddInstrumentFunction(Trace, 0);
    IMG_AddInstrumentFunction(Image, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
