//Groups: @ingroup\s+(API_REF|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_ARM|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_ARM|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|PIN_MEMORY|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|OPTIMIZATION_API|MISC|CHECKPOINTING|INST_ARGS|PIN_THREAD_API|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_ARM|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


/* DO NOT EDIT */

#define EnterPinClientSlaveMode() CheckPinClientLock(__FUNCTION__)
#define ExitPinClientSlaveMode() CheckPinClientLock(__FUNCTION__)
#define LOG_IMAGE(message)          QMESSAGE(MessageTypeLogImage, string("Image:  ")     + message)
#define LOG_SYM(message)            QMESSAGE(MessageTypeLogSym, string("Symbols:  ")     + message)

typedef INT32 THREADID;

                                                                  /* DO NOT EDIT */
class TRACE_CLASS;

                                                                  /* DO NOT EDIT */
typedef const TRACE_CLASS * TRACE;

                                                                  /* DO NOT EDIT */
class REGION;

                                                                  /* DO NOT EDIT */
class CHECKPOINT
{
  public:
    ADDRINT _pregs[NUM_PHYSICAL_REGS];     // physical registers
    ADDRINT _spillareaptr;                 // location of spill area
    ADDRINT _spillarea[NUM_SPILLED_REGS];  // content of spill area
    ADDRINT _fpregs[NUM_FP_REGS];          // floating point state
};

                                                                  /* DO NOT EDIT */
class CONTEXT
{
  public:
    ADDRINT _regs[NUM_PHYSICAL_REGS];      // architectural registers
    ADDRINT _fregs[NUM_FP_REGS];           // floating point state
};

                                                                  /* DO NOT EDIT */
typedef VOID (*REMOVE_INSTRUMENTATION_CALLBACK)(VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*DETACH_CALLBACK)(VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*FINI_CALLBACK)(INT32 code, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*FORK_CALLBACK)(INT32 threadid, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*THREAD_BEGIN_CALLBACK)(UINT32 threadIndex, VOID * sp, int flags, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*THREAD_END_CALLBACK)(UINT32 threadIndex, INT32 code, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*SIGNAL_BEFORE_CALLBACK)(INT32 threadIndex, INT32 sig, const CONTEXT *ctxtFrom,
    const CONTEXT *ctxtTo, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*SIGNAL_AFTER_CALLBACK)(INT32 threadIndex, const CONTEXT *ctxtFrom,
    const CONTEXT *ctxtTo, VOID *v);

                                                                  /* DO NOT EDIT */
typedef BOOL (*SIGNAL_INTERCEPT_CALLBACK)(INT32 sig, CONTEXT *ctxt, BOOL hasHndlr, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*INS_INSTRUMENT_CALLBACK)(INS ins, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*TRACE_INSTRUMENT_CALLBACK)(TRACE trace, VOID *v);

                                                                  /* DO NOT EDIT */
typedef VOID (*RTN_INSTRUMENT_CALLBACK)(RTN rtn, VOID *v);

                                                                  /* DO NOT EDIT */
extern VOID PIN_LockClient();

                                                                  /* DO NOT EDIT */
extern VOID PIN_UnlockClient();

                                                                  /* DO NOT EDIT */
extern VOID PIN_LockClient();

                                                                  /* DO NOT EDIT */
extern VOID PIN_UnlockClient();

                                                                  /* DO NOT EDIT */
extern VOID RTN_ReplaceWithUninstrumentedRoutine(RTN replacedRtn, AFUNPTR replacementFun);

                                                                  /* DO NOT EDIT */
extern VOID RTN_Replace(RTN replacedRtn, AFUNPTR replacementFun);

                                                                  /* DO NOT EDIT */
extern AFUNPTR RTN_ReplaceProbed(RTN replacedRtn, AFUNPTR replacementFun);

                                                                  /* DO NOT EDIT */
extern BOOL INS_IsAddedForFunctionReplacement(INS ins);

                                                                  /* DO NOT EDIT */
extern VOID PIN_RegisterItcAuxCallBackPushFun(AFUNPTR callBackPushFun);

                                                                  /* DO NOT EDIT */
extern VOID RTN_ComplexReplaceWithUninstrumentedRoutine(RTN replacedRtn, AFUNPTR replacementFun);

                                                                  /* DO NOT EDIT */
extern ADDRINT PIN_FindAlternateAppStack();

                                                                  /* DO NOT EDIT */
extern string PIN_DumpStackBounds();

                                                                  /* DO NOT EDIT */
extern VOID TRACE_AddInstrumentFunction(TRACE_INSTRUMENT_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID CODECACHE_AddTraceInsertedFunction(TRACE_INSTRUMENT_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID INS_AddInstrumentFunction(INS_INSTRUMENT_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddFiniFunction(FINI_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddDetachFunction(DETACH_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddThreadBeginFunction(THREAD_BEGIN_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddThreadEndFunction(THREAD_END_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddSignalBeforeFunction(SIGNAL_BEFORE_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddSignalAfterFunction(SIGNAL_AFTER_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddSignalInterceptFunction(INT32 sig, SIGNAL_INTERCEPT_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_AddForkFunction(FORK_CALLBACK fun, VOID *val);

                                                                  /* DO NOT EDIT */
extern VOID PIN_RemoveInstrumentation();

                                                                  /* DO NOT EDIT */
extern VOID PIN_FiniFunctionsCatchAppExit();

                                                                  /* DO NOT EDIT */
extern VOID PIN_RemoveFiniFunctions();

                                                                  /* DO NOT EDIT */
extern VOID PIN_Detach();

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertPredicatedCall(INS ins, IPOINT ipoint, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertCall(INS ins, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertIfCall(INS ins, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertThenCall(INS ins, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertIfPredicatedCall(INS ins, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID INS_InsertThenPredicatedCall(INS ins, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID PIN_StartProgram();

                                                                  /* DO NOT EDIT */
extern VOID PIN_StartProbedProgram();

                                                                  /* DO NOT EDIT */
extern BOOL ParseCommandLine(int xargc, CHAR **xargv);

                                                                  /* DO NOT EDIT */
extern BOOL  PIN_Init(INT32 argc, CHAR ** argv);

                                                                  /* DO NOT EDIT */
extern TRACE TRACE_Allocate(ADDRINT addr);

                                                                  /* DO NOT EDIT */
extern TRACE TRACE_AllocateForProbe(PROBE_TYPE probeType, ADDRINT addr);

                                                                  /* DO NOT EDIT */
extern VOID TRACE_Deallocate(TRACE trace);

                                                                  /* DO NOT EDIT */
extern VOID TRACE_InsertCall(TRACE trace, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID TRACE_InsertIfCall(TRACE trace, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID TRACE_InsertThenCall(TRACE trace, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern BBL TRACE_BblHead(TRACE trace);

                                                                  /* DO NOT EDIT */
extern BBL TRACE_BblTail(TRACE trace);

                                                                  /* DO NOT EDIT */
extern BOOL TRACE_Original(TRACE trace);

                                                                  /* DO NOT EDIT */
extern ADDRINT TRACE_Address(TRACE trace);

                                                                  /* DO NOT EDIT */
extern USIZE TRACE_Size(TRACE trace);

                                                                  /* DO NOT EDIT */
extern USIZE TRACE_OriginalSize(TRACE trace);

                                                                  /* DO NOT EDIT */
extern RTN TRACE_Rtn(TRACE trace);

                                                                  /* DO NOT EDIT */
extern BOOL TRACE_HasFallThrough(TRACE trace);

                                                                  /* DO NOT EDIT */
extern UINT32 TRACE_NumBbl(TRACE trace);

                                                                  /* DO NOT EDIT */
extern UINT32 TRACE_NumIns(TRACE trace);

                                                                  /* DO NOT EDIT */
extern VOID BBL_InsertCall(BBL bbl, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID BBL_InsertIfCall(BBL bbl, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern VOID BBL_InsertThenCall(BBL bbl, IPOINT action, AFUNPTR funptr, ...);

                                                                  /* DO NOT EDIT */
extern BOOL BBL_HasFallThrough(BBL bbl);

                                                                  /* DO NOT EDIT */
extern VOID PIN_SaveCheckpoint(CHECKPOINT * chkptFrom, CHECKPOINT * chkptTo);

                                                                  /* DO NOT EDIT */
extern VOID PIN_Resume(CHECKPOINT * chkpt);

                                                                  /* DO NOT EDIT */
extern VOID PIN_SetContextReg(CONTEXT * ctxt, const REG reg, const ADDRINT val);

                                                                  /* DO NOT EDIT */
extern ADDRINT PIN_GetContextReg(const CONTEXT * ctxt, const REG reg);

                                                                  /* DO NOT EDIT */
extern VOID PIN_SetContextFPState(CONTEXT * ctxt, VOID * fpstate);

                                                                  /* DO NOT EDIT */
extern VOID PIN_GetContextFPState(const CONTEXT * ctxt, VOID * fpstate);

                                                                  /* DO NOT EDIT */
extern VOID PIN_SaveContext(const CONTEXT * ctxtFrom, CONTEXT * ctxtTo);

                                                                  /* DO NOT EDIT */
extern VOID PIN_ExecuteAt(const CONTEXT * ctxt);

                                                                  /* DO NOT EDIT */
extern "C" VOID* DoJitThreadStartRoutine(THREAD_STARTROUTINE startroutine,
                                         VOID* arg,  VOID* threadTag, ADDRINT returnIp, ADDRINT newContextSp);

                                                                  /* DO NOT EDIT */
extern VOID* PIN_RegisterNewThread();

                                                                  /* DO NOT EDIT */
extern INT32 PIN_CreateThreadDataKey(DESTRUCTFUN destruct_func);

                                                                  /* DO NOT EDIT */
extern INT32 PIN_SetThreadData( INT32 key, const VOID * data );

                                                                  /* DO NOT EDIT */
extern VOID * PIN_GetThreadData( INT32 key );

                                                                  /* DO NOT EDIT */
extern VOID PIN_SetThreadSpecificKeyDestructor( INT32 key, DESTRUCTFUN destructFun );

                                                                  /* DO NOT EDIT */
extern BOOL INS_ChangeMemOpToBaseRegisterAddressMode(INS ins, MEMORY_TYPE mtype, REG newBase);

                                                                  /* DO NOT EDIT */
extern VOID GetVmLock();

                                                                  /* DO NOT EDIT */
extern VOID ReleaseVmLock();

                                                                  /* DO NOT EDIT */
extern UINT32 PIN_GetTid();

                                                                  /* DO NOT EDIT */
extern UINT32 PIN_GetParentTid();

                                                                  /* DO NOT EDIT */
extern VOID INS_Delete(INS ins);

                                                                  /* DO NOT EDIT */
extern VOID IARGLIST_AddArguments(IARGLIST args,  ...);

                                                                  /* DO NOT EDIT */
extern IARGLIST IARGLIST_Alloc();

                                                                  /* DO NOT EDIT */
extern VOID IARGLIST_Free(IARGLIST args);

                                                                  /* DO NOT EDIT */

