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
  @ORIGINAL_AUTHOR: Robert Muth and Artur Klauser
*/

/* ===================================================================== */
/*! @file
  *  This file contains a "real time" tool for showing the currently hostest routines
 */

#include "pin.H"
#include "portability.H"
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm> // for sort
#include <vector>

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,         "pintool",
                            "o", "funtop.out", "specify profile file name");

KNOB<UINT64>   KnobThreshold(KNOB_MODE_WRITEONCE,  "pintool",
                             "t", "10000", "print histogram after every n calls");

KNOB<UINT64>   KnobCutoff(KNOB_MODE_WRITEONCE,  "pintool",
                          "c", "100", "minimum count for routine to show");

KNOB<UINT64>   KnobMaxLines(KNOB_MODE_WRITEONCE,  "pintool",
                          "l", "24", "max number of output lines");

KNOB<UINT64>   KnobDetachUpdates(KNOB_MODE_WRITEONCE,  "pintool",
                          "d", "0", "detach after n screen updates");

KNOB<FLT64>   KnobDecayFactor(KNOB_MODE_WRITEONCE,  "pintool",
                          "f", "0.0", "x");



/* ===================================================================== */

INT32 Usage()
{
    cerr <<
         "This pin tool \"real time\" tool for showing the currently hostest routines\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}



string invalid = "invalid_rtn";

/* ===================================================================== */
const string *Target2String(ADDRINT target)
{
    string name = RTN_FindNameByAddress(target);
    if (name == "")
        return &invalid;
    else
        return new string(name);
}

/* ===================================================================== */

typedef  map<ADDRINT,UINT64> ADDR_CNT_MAP;
typedef pair<ADDRINT,UINT64> PAIR;
typedef vector<PAIR> VEC;

LOCALVAR ADDR_CNT_MAP RtnMap;


/* ===================================================================== */

LOCALVAR UINT64 counter = 0;
LOCALVAR UINT64 updates = 0;

std::ofstream Out;

LOCALFUN BOOL CompareLess(PAIR  s1 , PAIR  s2)
{
        return s1.second >  s2.second;
}

/* ===================================================================== */
VOID DumpHistogram(std::ostream& out)
{
    const UINT64 cutoff = KnobCutoff.Value();
    const UINT64 maxlines = KnobMaxLines.Value();
    FLT64 factor = KnobDecayFactor.Value();

    out << "\033[0;0H";
    out << "\033[2J";
    out << "\033[44m";
    out << "Functions with at least " << cutoff << " invocations in the last " <<
        KnobThreshold.Value() << " calls ";
    out << "\033[0m";
    out << endl;

    
    VEC CountMap;
    
    for (ADDR_CNT_MAP::iterator bi = RtnMap.begin(); bi != RtnMap.end(); bi++)
    {
        if( bi->second < cutoff ) continue;

        CountMap.push_back(*bi);
#if 0
        out << setw(18) << (void *)(bi->first) << " " <<
            setw(10) << bi->second <<
            "   " << Target2String(bi->first) << endl;
#endif
    }

    sort( CountMap.begin(), CountMap.end(), CompareLess );
    UINT64 lines = 0;
    for (VEC::iterator bi = CountMap.begin(); bi != CountMap.end(); bi++)
    {
        out << setw(18) << (void *)(bi->first) << " " <<
            setw(10) << bi->second <<
            "   " << Target2String(bi->first) << endl;
        lines++;
        if (lines >= maxlines) break;
    }
    
    for (ADDR_CNT_MAP::iterator bi = RtnMap.begin(); bi != RtnMap.end(); bi++)
    {
        bi->second = UINT64(bi->second * factor);
    }
    
    //out << "Total Functions: " << CountMap.size() << endl;
    
}


/* ===================================================================== */

VOID  do_call_indirect(ADDRINT target, BOOL taken)
{
    if( !taken ) return;

    if( counter == 0 )
    {
        DumpHistogram(Out);
        Out << flush;

        counter = KnobThreshold.Value();
        updates++;
        if ( updates == KnobDetachUpdates.Value() )
        {
            PIN_Detach();
        }
    }

    counter--;

    RtnMap[target]++;
}

/* ===================================================================== */

VOID Trace(TRACE trace, VOID *v)
{
        
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        INS tail = BBL_InsTail(bbl);
        
        if( INS_IsCall(tail) )
        {
            INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                               IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_END);
        }
        else
        {
            // On em64t, some of the code is not in an image
            RTN rtn = TRACE_Rtn(trace);
            
            // also track stup jumps into share libraries
            if( RTN_Valid(rtn) && !INS_IsDirectBranchOrCall(tail) && ".plt" == SEC_Name( RTN_Sec( rtn ) ))
            {
                INS_InsertCall(tail, IPOINT_BEFORE, AFUNPTR(do_call_indirect),
                               IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_END);
            }
        }
        
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }

    Out.open(KnobOutputFile.Value().c_str());
    TRACE_AddInstrumentFunction(Trace, 0);

    // Never returns

    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
