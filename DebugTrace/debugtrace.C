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
  @ORIGINAL_AUTHOR: Robert Cohn
*/

/* ===================================================================== */
/*! @file
 *  This file contains a tool that generates instructions traces with values.
 *  It is designed to help debugging.
 */



#include "pin.H"
#include "instlib.H"
#include "time_warp.H"
#include "portability.H"
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace INSTLIB;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,         "pintool",
    "o", "debugtrace.out", "trace file");
KNOB<BOOL>   KnobPid(KNOB_MODE_WRITEONCE,                "pintool",
    "i", "0", "append pid to output");
KNOB<INT32>   KnobWatchThread(KNOB_MODE_WRITEONCE,                "pintool",
    "watch_thread", "-1", "thread to watch, -1 for all");
KNOB<BOOL>   KnobFlush(KNOB_MODE_WRITEONCE,                "pintool",
    "flush", "0", "Flush output after every instruction");
KNOB<BOOL>   KnobSymbols(KNOB_MODE_WRITEONCE,       "pintool",
    "symbols", "1", "Include symbol information");
KNOB<BOOL>   KnobLines(KNOB_MODE_WRITEONCE,       "pintool",
    "lines", "0", "Include line number information");
KNOB<BOOL>   KnobTraceInstructions(KNOB_MODE_WRITEONCE,       "pintool",
    "instruction", "0", "Trace instructions");
KNOB<BOOL>   KnobTraceCalls(KNOB_MODE_WRITEONCE,       "pintool",
    "call", "1", "Trace calls");
KNOB<BOOL>   KnobTraceMemory(KNOB_MODE_WRITEONCE,       "pintool",
    "memory", "0", "Trace memory");
KNOB<BOOL>   KnobSilent(KNOB_MODE_WRITEONCE,       "pintool",
    "silent", "0", "Do everything but write file (for debugging).");
KNOB<BOOL> KnobEarlyOut(KNOB_MODE_WRITEONCE, "pintool", "early_out", "0" , "Exit after tracing the first region.");


/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool collects an instruction trace for debugging\n"
        "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

typedef UINT64 COUNTER;

LOCALVAR std::ofstream out;

LOCALVAR INT32 enabled = 0;

LOCALVAR FILTER filter;

LOCALVAR ICOUNT icount;

LOCALFUN BOOL Emit(INT32 threadid)
{
    if (!enabled || KnobSilent || (KnobWatchThread != -1 && KnobWatchThread != threadid))
        return false;
    return true;
}

LOCALFUN VOID Flush()
{
    if (KnobFlush)
        out << flush;
}

/* ===================================================================== */

LOCALFUN VOID Fini(int, VOID * v);

LOCALFUN VOID Handler(CONTROL_EVENT ev, VOID *, CONTEXT * ctxt, VOID *, VOID *)
{
    switch(ev)
    {
      case CONTROL_START:
        enabled = 1;
        PIN_RemoveInstrumentation();
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    // So that the rest of the current trace is re-instrumented.
    if (ctxt) PIN_ExecuteAt (ctxt);
#endif   
        break;

      case CONTROL_STOP:
        enabled = 0;
        PIN_RemoveInstrumentation();
        if (KnobEarlyOut)
        {
            cerr << "Exiting due to -early_out" << endl;
            Fini(0, NULL);
            exit(0);
        }
#if defined(TARGET_IA32) || defined(TARGET_IA32E)
    // So that the rest of the current trace is re-instrumented.
    if (ctxt) PIN_ExecuteAt (ctxt);
#endif   
        break;

      default:
        ASSERTX(false);
    }
}


/* ===================================================================== */

VOID EmitNoValues(INT32 threadid, string * str)
{
    if (!Emit(threadid))
        return;
    
    out
        << *str
        << endl;

    Flush();
}

VOID Emit1Values(INT32 threadid, string * str, string * reg1str, ADDRINT reg1val)
{
    if (!Emit(threadid))
        return;
    
    out
        << *str << " | "
        << *reg1str << " = " << reg1val
        << endl;

    Flush();
}

VOID Emit2Values(INT32 threadid, string * str, string * reg1str, ADDRINT reg1val, string * reg2str, ADDRINT reg2val)
{
    if (!Emit(threadid))
        return;
    
    out
        << *str << " | "
        << *reg1str << " = " << reg1val
        << ", " << *reg2str << " = " << reg2val
        << endl;
    
    Flush();
}

VOID Emit3Values(INT32 threadid, string * str, string * reg1str, ADDRINT reg1val, string * reg2str, ADDRINT reg2val, string * reg3str, ADDRINT reg3val)
{
    if (!Emit(threadid))
        return;
    
    out
        << *str << " | "
        << *reg1str << " = " << reg1val
        << ", " << *reg2str << " = " << reg2val
        << ", " << *reg3str << " = " << reg3val
        << endl;
    
    Flush();
}


VOID Emit4Values(INT32 threadid, string * str, string * reg1str, ADDRINT reg1val, string * reg2str, ADDRINT reg2val, string * reg3str, ADDRINT reg3val, string * reg4str, ADDRINT reg4val)
{
    if (!Emit(threadid))
        return;
    
    out
        << *str << " | "
        << *reg1str << " = " << reg1val
        << ", " << *reg2str << " = " << reg2val
        << ", " << *reg3str << " = " << reg3val
        << ", " << *reg4str << " = " << reg4val
        << endl;
    
    Flush();
}


const UINT32 MaxEmitArgs = 4;

AFUNPTR emitFuns[] = 
{
    AFUNPTR(EmitNoValues), AFUNPTR(Emit1Values), AFUNPTR(Emit2Values), AFUNPTR(Emit3Values), AFUNPTR(Emit4Values)
};

/* ===================================================================== */
VOID AddEmit(INS ins, IPOINT point, string & traceString, UINT32 regCount, REG regs[])
{
    if (regCount > MaxEmitArgs)
        regCount = MaxEmitArgs;
    
    IARGLIST args = IARGLIST_Alloc();
    for (UINT32 i = 0; i < regCount; i++)
    {
        IARGLIST_AddArguments(args, IARG_PTR, new string(REG_StringShort(regs[i])), IARG_REG_VALUE, regs[i], IARG_END);
    }

    INS_InsertCall(ins, point, emitFuns[regCount], IARG_THREAD_ID,
                   IARG_PTR, new string(traceString),
                   IARG_IARGLIST, args,
                   IARG_END);
    IARGLIST_Free(args);
}

LOCALVAR VOID *WriteEa[1000];

VOID CaptureWriteEa(INT32 threadid, VOID * addr)
{
    WriteEa[threadid] = addr;
}

VOID ShowN(UINT32 n, VOID *ea)
{
    out.unsetf(ios::showbase);
    out << "0x";
    for (UINT32 i = 0; i < n; i++)
    {
        out << static_cast<UINT32>(static_cast<UINT8*>(ea)[i]);
    }
    out.setf(ios::showbase);
}


VOID EmitWrite(INT32 threadid, UINT32 size)
{
    if (!Emit(threadid))
        return;
    
    out << "                                 Write ";
    
    VOID * ea = WriteEa[threadid];
    
    switch(size)
    {
      case 0:
        out << "0 repeat count" << endl;
        break;
        
      case 1:
        out << "*(UINT8*)" << ea << " = " << static_cast<UINT32>(*static_cast<UINT8*>(ea)) << endl;
        break;
        
      case 2:
        out << "*(UINT16*)" << ea << " = " << *static_cast<UINT16*>(ea) << endl;
        break;
        
      case 4:
        out << "*(UINT32*)" << ea << " = " << *static_cast<UINT32*>(ea) << endl;
        break;
        
      case 8:
        out << "*(UINT64*)" << ea << " = " << *static_cast<UINT64*>(ea) << endl;
        break;
        
      default:
        out << "*(UINT" << dec << size * 8 << hex << ")" << ea << " = ";
        ShowN(size,ea);
        out << endl;
        break;
    }

    Flush();
}

VOID EmitRead(INT32 threadid, VOID * ea, UINT32 size)
{
    if (!Emit(threadid))
        return;
    
    out << "                                 Read ";

    switch(size)
    {
      case 0:
        out << "0 repeat count" << endl;
        break;
        
      case 1:
        out << static_cast<UINT32>((*static_cast<UINT8*>(ea))) << " = *(UINT8*)" << ea << endl;
        break;
        
      case 2:
        out << *static_cast<UINT16*>(ea) << " = *(UINT16*)" << ea << endl;
        break;
        
      case 4:
        out << *static_cast<UINT32*>(ea) << " = *(UINT32*)" << ea << endl;
        break;
        
      case 8:
        out << *static_cast<UINT64*>(ea) << " = *(UINT64*)" << ea << endl;
        break;
        
      default:
        ShowN(size,ea);
        out << " = *(UINT" << dec << size * 8 << hex << ")" << ea << endl;
        break;
    }

    Flush();
}


LOCALVAR INT32 indent = 0;

VOID Indent()
{
    for (INT32 i = 0; i < indent; i++)
    {
        out << "| ";
    }
}

VOID EmitICount()
{
    out << setw(10) << dec << icount.Count() << hex << " ";
}

VOID EmitDirectCall(INT32 threadid, string * str, INT32 tailCall, ADDRINT arg0, ADDRINT arg1)
{
    if (!Emit(threadid))
        return;
    
    EmitICount();

    if (tailCall)
    {
        // A tail call is like an implicit return followed by an immediate call
        indent--;
    }
    
    Indent();
    out << *str << "(" << arg0 << ", " << arg1 << ", ...)" << endl;

    indent++;

    Flush();
}

string FormatAddress(ADDRINT address, RTN rtn)
{
    string s = "";
    
    if (KnobSymbols && RTN_Valid(rtn))
    {
        s += RTN_Name(rtn);

        ADDRINT delta = address - RTN_Address(rtn);
        if (delta != 0)
        {
            s += "+" + hexstr(delta, 4);
        }

    }
    else
    {
        s = StringFromAddrint(address);
    }

    if (KnobLines)
    {
        INT32 line;
        const CHAR * file;
        
        PIN_FindLineFileByAddress(address, &line, &file);

        if (file)
        {
            s += " (" + string(file) + ":" + decstr(line) + ")";
        }
    }
    return s;
}

VOID EmitIndirectCall(INT32 threadid, string * str, ADDRINT target, ADDRINT arg0, ADDRINT arg1)
{
    if (!Emit(threadid))
        return;
    
    EmitICount();
    Indent();
    out << *str;

    PIN_LockClient();
    
    string s = FormatAddress(target, RTN_FindByAddress(target));
    
    PIN_UnlockClient();
    
    out << s << "(" << arg0 << ", " << arg1 << ", ...)" << endl;
    indent++;

    Flush();
}

VOID EmitReturn(INT32 threadid, string * str, ADDRINT ret0)
{
    if (!Emit(threadid))
        return;
    
    EmitICount();
    indent--;
    if (indent < 0)
    {
        out << "@@@ return underflow\n";
        indent = 0;
    }
    
    Indent();
    out << *str << " returns: " << ret0 << endl;

    Flush();
}

        
VOID CallTrace(TRACE trace, INS ins)
{
    if (!KnobTraceCalls)
        return;

    if (INS_IsCall(ins) && !INS_IsDirectBranchOrCall(ins))
    {
        // Indirect call
        string s = "Call " + FormatAddress(INS_Address(ins), TRACE_Rtn(trace));
        s += " -> ";

        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(EmitIndirectCall), IARG_THREAD_ID,
                       IARG_PTR, new string(s), IARG_BRANCH_TARGET_ADDR,
                       IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_END);
    }
    else if (INS_IsDirectBranchOrCall(ins))
    {
        // Is this a tail call?
        RTN sourceRtn = TRACE_Rtn(trace);
        RTN destRtn = RTN_FindByAddress(INS_DirectBranchOrCallTargetAddress(ins));

        if (INS_IsCall(ins)         // conventional call
            || sourceRtn != destRtn // tail call
        )
        {
            BOOL tailcall = !INS_IsCall(ins);
            
            string s = "";
            if (tailcall)
            {
                s += "Tailcall ";
            }
            else
            {
                if( INS_IsProcedureCall(ins) )
                    s += "Call ";
                else
                {
                    s += "PcMaterialization ";
                    tailcall=1;
                }
                
            }

            //s += INS_Mnemonic(ins) + " ";
            
            s += FormatAddress(INS_Address(ins), TRACE_Rtn(trace));
            s += " -> ";

            ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
        
            s += FormatAddress(target, RTN_FindByAddress(target));

            INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(EmitDirectCall),
                           IARG_THREAD_ID, IARG_PTR, new string(s), IARG_UINT32, tailcall,
                           IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_END);
        }
    }
    else if (INS_IsRet(ins))
    {
        RTN rtn =  TRACE_Rtn(trace);
        
#if defined(TARGET_LINUX) && defined(TARGET_IA32)
//        if( RTN_Name(rtn) ==  "_dl_debug_state") return;
        if( RTN_Name(rtn) ==  "_dl_runtime_resolve") return;
#endif
        string tracestring = "Return " + FormatAddress(INS_Address(ins), rtn);
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(EmitReturn),
                       IARG_THREAD_ID, IARG_PTR, new string(tracestring), IARG_G_RESULT0, IARG_END);
    }
}
        

VOID InstructionTrace(TRACE trace, INS ins)
{
    if (!KnobTraceInstructions)
        return;
    
    ADDRINT addr = INS_Address(ins);
    ASSERTX(addr);
            
    // Format the string at instrumentation time
    string traceString = "";
    string astring = FormatAddress(INS_Address(ins), TRACE_Rtn(trace));
    for (INT32 length = astring.length(); length < 30; length++)
    {
        traceString += " ";
    }
    traceString = astring + traceString;
    
    traceString += " " + INS_Disassemble(ins);

    for (INT32 length = traceString.length(); length < 80; length++)
    {
        traceString += " ";
    }

    INT32 regCount = 0;
    REG regs[20];
            
    for (UINT32 i = 0; i < INS_MaxNumWRegs(ins); i++)
    {
        REG x = REG_FullRegName(INS_RegW(ins, i));
        if (REG_is_gr(x) 
#if defined(TARGET_IA32)
            || x == REG_EFLAGS
#elif defined(TARGET_IA32E)
            || x == REG_RFLAGS
#elif defined(TARGET_IPF)
            || REG_is_pr(x)
            || REG_is_br(x)
#endif
        )
        {
            regs[regCount] = x;
            regCount++;
        }
    }

    if (INS_HasFallThrough(ins))
    {
        AddEmit(ins, IPOINT_AFTER, traceString, regCount, regs);
    }
    if (INS_IsBranchOrCall(ins))
    {
        AddEmit(ins, IPOINT_TAKEN_BRANCH, traceString, regCount, regs);
    }
}

VOID MemoryTrace(INS ins)
{
    if (!KnobTraceMemory)
        return;
    
    if (INS_IsMemoryWrite(ins))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(CaptureWriteEa), IARG_THREAD_ID, IARG_MEMORYWRITE_EA, IARG_END);

        if (INS_HasFallThrough(ins))
        {
            INS_InsertPredicatedCall(ins, IPOINT_AFTER, AFUNPTR(EmitWrite), IARG_THREAD_ID, IARG_MEMORYWRITE_SIZE, IARG_END);
        }
        if (INS_IsBranchOrCall(ins))
        {
            INS_InsertPredicatedCall(ins, IPOINT_TAKEN_BRANCH, AFUNPTR(EmitWrite), IARG_THREAD_ID, IARG_MEMORYWRITE_SIZE, IARG_END);
        }
    }

    if (INS_HasMemoryRead2(ins))
    {
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(EmitRead), IARG_THREAD_ID, IARG_MEMORYREAD2_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }

    if (INS_IsMemoryRead(ins) && !INS_IsPrefetch(ins))
    {
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(EmitRead), IARG_THREAD_ID, IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }
}


/* ===================================================================== */

VOID Trace(TRACE trace, VOID *v)
{
    if (!filter.SelectTrace(trace))
        return;
    
    if (enabled)
    {
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
            {
                InstructionTrace(trace, ins);
    
                CallTrace(trace, ins);
    
                MemoryTrace(ins);
            }
        }
    }
}


/* ===================================================================== */

VOID Fini(int, VOID * v)
{
    out << "# $eof" <<  endl;

    out.close();
}


LOCALVAR CONTROL control;
LOCALVAR TIME_WARP tw;
    

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    string filename =  KnobOutputFile.Value();

    if( KnobPid )
    {
        filename += "." + decstr( getpid_portable() );
    }

    // Do this before we activate controllers
    out.open(filename.c_str());
    out << hex << right;
    out.setf(ios::showbase);

    tw.CheckKnobs(0); // Want time_warp action any other instrumentation
    control.CheckKnobs(Handler, 0);
    
    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_AddFiniFunction(Fini, 0);

    filter.Activate();
    icount.Activate();
    
    // Never returns

    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
