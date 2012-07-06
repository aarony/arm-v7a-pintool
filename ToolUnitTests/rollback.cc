/* ================================================================== */
/* Checkpointing Pin Tool                                             */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* functionality:                                                     */
/* checkpoints at a particular routine (#rtn_save) and resumes at     */
/* the checkpointed routine when it first hits the later routine      */
/* (#rtn_resume); changes to memory are logged from the checkpoint,   */
/* and restored before resuming                                       */
/*                                                                    */
/* command line options:                                              */
/* -save <INT>    sets rtn_save (defaults to random number)           */
/* -resume <INT>  sets rtn_resume (defaults to random number)         */
/* -maxrtn <INT>  sets rtn_max (# of dynamically executed routine     */
/*                in the application - defaults to 800)               */
/* -usectxt       use architectural state instead of proc/pin state   */
/*                (also dumps the arch & mem state into files for     */
/*                later use)                                          */
/*                                                                    */
/* ================================================================== */

#include "utilities.h"
#include "memlog.h"
#include <time.h>

/* ------------------------------------------------------------------ */
/* Global Variables                                                   */
/* ------------------------------------------------------------------ */

CHECKPOINT chkpt;      // saved processor state
CONTEXT ctxt;          // saved architectural state
MEMLOG* memlog;        // saved memory state
UINT32 rtn_current;    // counter dynamically tracking which routine we're executing
UINT32 rtn_save;       // the routine at which we save the checkpoint
UINT32 rtn_resume;     // the routine at which we resume at the saved checkpoint
UINT32 rtn_max;        // the last routine we consider
BOOL verbose;          // whether to output trace file
fstream tracefile;     // trace file
BOOL forcedresume;     // whether syscall before rtn_resume has forced an earlier resume
BOOL usectxt;          // whether to use the architectural state instead of proc/pin state
fstream ctxtfile;      // file saving the context
fstream memfile;       // file saving the memory log
BOOL firstins;         // whether we are at the first (memwrite) instruction
ADDRINT orig_sp;       // stack pointer at the beginning of the program

/* ------------------------------------------------------------------ */
/* Function Declarations                                              */
/* ------------------------------------------------------------------ */

VOID Init(UINT32, char**);
VOID Fini(INT32, VOID*);
VOID FlagRtn(RTN, VOID*);
VOID FlagIns(INS, VOID*);
VOID Checkpointing(const string*, CHECKPOINT*, CONTEXT*, BOOL);
VOID ForceResume(const string*);
VOID LogMemWrite(ADDRINT, UINT32);
VOID LogInsTrace(ADDRINT, ADDRINT, const string*);

/* ================================================================== */

int main(UINT32 argc, char** argv) 
{
    Init(argc, argv);
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    INS_AddInstrumentFunction(FlagIns, 0);
    RTN_AddInstrumentFunction(FlagRtn, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
}

/* ------------------------------------------------------------------ */
/* Initalization & Clean Up                                           */
/* ------------------------------------------------------------------ */

VOID Init(UINT32 argc, char** argv) 
{
    srand(time(NULL));
    rtn_current = 0;
    GetArg(argc, argv, "-maxrtn",  rtn_max,    800);
    GetArg(argc, argv, "-save",    rtn_save,   rand() % rtn_max);
    (rtn_max > rtn_save) ?
        GetArg(argc, argv, "-resume",  rtn_resume, rand() % (rtn_max - rtn_save) + rtn_save) :
        GetArg(argc, argv, "-resume",  rtn_resume, rand() % rtn_save + rtn_save);
    std::cout << "routines - save: " << rtn_save << ", resume: " << rtn_resume << "\n" << flush;
    GetArg(argc, argv, "-traceon", verbose);
    memlog = new MEMLOG(verbose);
    if (verbose) 
    {
        tracefile.open("checkpoint.txt", fstream::out | fstream::app);
    }
    forcedresume = false;
    GetArg(argc, argv, "-usectxt", usectxt);
    if (usectxt) 
    {
        ctxtfile.open("ctxtsave.txt", fstream::out);
        memfile.open("memsave.txt", fstream::out);
    }
    firstins = true;
}

VOID Fini(INT32 code, VOID* v) 
{
    delete memlog;
    if (verbose) 
    {
        tracefile.close();
    }
    if (usectxt) 
    {
        ctxtfile.close();
        memfile.close();
    }
}

/* ------------------------------------------------------------------ */
/* Instrumentation Routines                                           */
/* ------------------------------------------------------------------ */

VOID FlagRtn(RTN rtn, VOID* v) 
{
    RTN_Open(rtn);
    // Checkpointing will checkpoint at rtn_save and resume at that checkpoint upon reaching rtn_resume
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)Checkpointing,
                   IARG_PTR, new string(RTN_Name(rtn)),
                   IARG_CHECKPOINT,
                   IARG_CONTEXT,
                   IARG_BOOL, false,
                   IARG_END);
    RTN_Close(rtn);
}

VOID FlagIns(INS ins, VOID* v) 
{
    // if a syscall occurs after checkpointing, we must resume earlier
    if (INS_IsSyscall(ins)) 
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)ForceResume,
                       IARG_PTR, new string(RTN_Name(INS_Rtn(ins))),
                       IARG_END);
    }
    // send all memory write instructions to LogMemWrite so we can log changes to memory to undo when resuming
    if (INS_IsMemoryWrite(ins))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)LogMemWrite,
                       IARG_MEMORYWRITE_EA,
                       IARG_MEMORYWRITE_SIZE,
                       IARG_END);
    }
    // sanity check to make sure we have all instructions that will affect the memory
    else 
    {
        ASSERTX(!INS_IsStackWrite(ins));
        ASSERTX(!INS_IsCall(ins));
    }
    // send all instructions to LogInsTrace if we want to an output trace file
    if (verbose || firstins) 
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)LogInsTrace,
                       IARG_INST_PTR,
                       IARG_REG_VALUE, REG_STACK_PTR,
                       IARG_PTR, new string(INS_Disassemble(ins)),
                       IARG_END);
    }
}

/* ------------------------------------------------------------------ */
/* Checkpointing:                                                     */
/* depending on where we are during execution, determines whether we  */
/* should checkpoint the processor state or resume at the checkpoint  */ 
/* ------------------------------------------------------------------ */

VOID Checkpointing(const string* rtn_name, CHECKPOINT* _chkpt, CONTEXT* _ctxt, BOOL mustresume) 
{
    if (rtn_current == rtn_save) 
    {
        std::cout << "saving at RTN " << *rtn_name << "\n" << flush;
        if (verbose) 
        {
            tracefile << "----------Saving Checkpoint----------\n" << flush;
            tracefile << "RTN " << *rtn_name << "\n" << flush;
        }
        if (usectxt) 
        {
            memlog->DumpMemState(memfile, tracefile, orig_sp);
            PIN_SaveContext(_ctxt, &ctxt);
            for (REG reg = REG_PHYSICAL_CONTEXT_BEGIN; reg <= REG_PHYSICAL_CONTEXT_END; reg = REG(reg + 1)) 
            {
                ctxtfile << hex << PIN_GetContextReg(&ctxt, reg) << "\n" << flush;
            }
        }
        else 
        {
            PIN_SaveCheckpoint(_chkpt, &chkpt);
        }
        rtn_current++;
    }
    else if ((rtn_current == rtn_resume) && !forcedresume)
    {
        if (usectxt) 
        {
            std::cout << "executing at RTN " << *rtn_name << "\n" << flush;
        }
        else 
        {
            std::cout << "resuming at RTN " << *rtn_name << "\n" << flush;
        }
        if (verbose) 
        {
            if (usectxt) 
            {
                tracefile << "----------ExecutingAt----------\n" << flush;
            }
            else 
            {
                tracefile << "----------Resuming----------\n" << flush;
            }
            tracefile << "RTN " << *rtn_name << "\n" << flush;
        }
        memlog->RestoreMem(tracefile);
        rtn_current++;
        if (usectxt) 
        {
            PIN_ExecuteAt(&ctxt);
        }
        else 
        {
            PIN_Resume(&chkpt);
        }
    }
    else if (mustresume) 
    {
        std::cout << "forced to resume (due to syscall) at RTN #" << dec 
                  << rtn_current - 1 << ": " << *rtn_name << "\n" << flush;
        if (verbose) 
        {
            tracefile << "----------Resuming----------\n" << flush;
            tracefile << "RTN " << *rtn_name << "\n" << flush;
        }
        memlog->RestoreMem(tracefile);
        forcedresume = true;
        // do not increment rtn_current b/c we have already counted this routine
        if (usectxt) 
        {
            PIN_ExecuteAt(&ctxt);
        }
        else 
        {
            PIN_Resume(&chkpt);
        }
    }
    else 
    {
        tracefile << "RTN " << *rtn_name << "\n" << flush;
        rtn_current++;
    }
}

/* ------------------------------------------------------------------ */
/* ForceResume:                                                       */
/* if an unsafe (non-replayable) syscall happens after rtn_save but   */
/* before rtn_resume, we must resume before the syscall               */
/* ------------------------------------------------------------------ */

VOID ForceResume(const string* rtn_name) 
{
    if ((rtn_current > rtn_save) && (rtn_current <= rtn_resume) && !forcedresume)
    {
        if ((rtn_name->find("sched") != string::npos) ||    // assume these syscalls are unsafe to replay
            (rtn_name->find("set") != string::npos) ||
            ((rtn_name->find("write") == string::npos) &&   // assume these syscalls are safe to replay (conservative)
             (rtn_name->find("access") == string::npos) &&
             (rtn_name->find("ftime") == string::npos) &&
             (rtn_name->find("stat") == string::npos) &&
             (rtn_name->find("sysinfo") == string::npos) &&
             (rtn_name->find("uname") == string::npos) &&
             (rtn_name->find("get") == string::npos)))
        {
            Checkpointing(rtn_name, NULL, NULL, true);
        }
        else 
        {
            std::cout << "ignoring re-executable syscall: " << *rtn_name << "\n" << flush;
        }
    }
        
}

/* ------------------------------------------------------------------ */
/* LogMemWrite:                                                       */
/* records the original memory state to restore to                    */
/* (only need to track mem locs written to since the checkpoint)      */
/* ------------------------------------------------------------------ */

VOID LogMemWrite(ADDRINT addr, UINT32 len)
{
    if (((rtn_current > rtn_save) && (rtn_current <= rtn_resume) && !forcedresume) ||
        ((rtn_current <= rtn_save) && usectxt))
    {
        memlog->RecordWrite(addr, len, tracefile);
    }
    else 
    {
        tracefile << "Not Saving Memory\n" << flush;
    }
}

/* ------------------------------------------------------------------ */
/* LogInsTrace:                                                       */
/* records the instructions and some state (stack pointer and         */
/* the value at the top of the stack)                                 */
/* ------------------------------------------------------------------ */

VOID LogInsTrace(ADDRINT ip, ADDRINT sp, const string* disassm) 
{
    if (firstins) 
    {
        orig_sp = sp;
        firstins = false;
    }
    tracefile << hex << ip << "\t" << sp << "\t" << flush;
    PrintHexWord(sp, tracefile);
    tracefile << "\t" << *disassm << "\n";
}

/* ------------------------------------------------------------------ */
