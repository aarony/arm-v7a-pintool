//Groups: @ingroup\s+(API_REF|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_ARM|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_ARM|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|PIN_MEMORY|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|OPTIMIZATION_API|MISC|CHECKPOINTING|INST_ARGS|PIN_THREAD_API|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_ARM|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


typedef INT32 PIN_LOCK;

                                                                  /* DO NOT EDIT */
enum 
{
    // key 0 is hardwired to be used by malloc?
    KEY_USER_MIN = 140,  // key KEY_USER_MIN to KEY_USER_MAX can be allocated by user
    KEY_USER_MAX = 190,
    KEY_OS_PARENT_TID = 197,     // OS thread id or process id of parent
    KEY_OS_TID = 198,     // OS thread id or process id
    KEY_THREADID = 199,    // Pin's thread id
    KEY_LAST = 200
};

                                                                  /* DO NOT EDIT */
extern VOID InitLock(PIN_LOCK * lock);

                                                                  /* DO NOT EDIT */
extern VOID GetLock(PIN_LOCK * lock, INT32 val);

                                                                  /* DO NOT EDIT */
extern INT32 ReleaseLock(PIN_LOCK * lock);

                                                                  /* DO NOT EDIT */

