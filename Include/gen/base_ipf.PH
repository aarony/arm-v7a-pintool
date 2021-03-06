//Groups: @ingroup\s+(API_REF|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_ARM|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_ARM|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|PIN_MEMORY|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|OPTIMIZATION_API|MISC|CHECKPOINTING|INST_ARGS|PIN_THREAD_API|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_ARM|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


typedef enum
{
    CATEGORY_INVALID,
    CATEGORY_MEMORY_FENCE,
    CATEGORY_LOAD,
    CATEGORY_STORE,
    CATEGORY_FETCH,
    CATEGORY_FETCHADD,
    CATEGORY_CMPXCHG,
    CATEGORY_XCHG,

    // control flow changing

    CATEGORY_BRANCH,
    CATEGORY_CBRANCH,   // Call not conditional!!
    CATEGORY_JUMP,
    CATEGORY_CJUMP,     // Call not conditional!!
    CATEGORY_CHECK,
    CATEGORY_RFI,
    CATEGORY_BREAK,

    CATEGORY_CALL_SIDE_EFFECTS,  // side effects of call, no control transfer (pseudo op)
    CATEGORY_RET_SIDE_EFFECTS,   // side effects of ret, no control transfer (pseudo op)

    CATEGORY_BRANCH_PREDICT,
    CATEGORY_JUMP_PREDICT,

    CATEGORY_RSE_CONTROL,
    
    CATEGORY_NOP,
    CATEGORY_HINT,
    CATEGORY_MOVL,
    CATEGORY_INT_OP,
    CATEGORY_VEC_OP,
    CATEGORY_ADDL,
    CATEGORY_ADDS,
    CATEGORY_INT_CMP,
    CATEGORY_INT_PCMP,
    CATEGORY_FLT_CMP,
    CATEGORY_FLT_PCMP,
    CATEGORY_TEST,
    CATEGORY_ALLOC,

    CATEGORY_GET_FR,
    CATEGORY_SET_FR,

    CATEGORY_SET_AR,
    CATEGORY_GET_AR,

    CATEGORY_GET_BR,
    CATEGORY_SET_BR,

    CATEGORY_GET_PR,
    CATEGORY_SET_PR,

    CATEGORY_IP2GR,

    CATEGORY_SET_CR,
    CATEGORY_GET_CR,

    CATEGORY_SET_MR,
    CATEGORY_GET_MR,

    CATEGORY_GET_PSR,
    CATEGORY_SET_PSR,

    CATEGORY_GET_MASK,
    CATEGORY_SET_MASK,

    CATEGORY_PURGE_TRANSLATION_CACHE,
    CATEGORY_TRANSLATION,

    CATEGORY_INT_DEP,
    CATEGORY_INT_EXT,
    CATEGORY_SHIFT,
    CATEGORY_FLT_SQRT,
    CATEGORY_FLT_DIV,

    CATEGORY_CLEAR_RRB,
    CATEGORY_COVER,
    CATEGORY_EPC,
    CATEGORY_BSW,

    CATEGORY_BMISC,


    CATEGORY_MMISC,
    CATEGORY_FLT_OP,
    CATEGORY_FLT_CVT,
    CATEGORY_FLT_STATUS,
    CATEGORY_FLT_CLASS,
    CATEGORY_FLT_MULACC,
    CATEGORY_FLT_SELECT,
    CATEGORY_LAST
}CATEGORY_IPF;

                                                                  /* DO NOT EDIT */
typedef enum
{
    PREDICATE_PR0 = REG_PBASE + 0,
    PREDICATE_PR1 = REG_PBASE + 1,
    PREDICATE_PR2 = REG_PBASE + 2,
    PREDICATE_PR3 = REG_PBASE + 3,
    PREDICATE_PR4 = REG_PBASE + 4,
    PREDICATE_PR5 = REG_PBASE + 5,
    PREDICATE_PR6 = REG_PBASE + 6,
    PREDICATE_PR7 = REG_PBASE + 7,
    PREDICATE_PR8 = REG_PBASE + 8,
    PREDICATE_PR9 = REG_PBASE + 9,
    PREDICATE_PR15 = REG_PBASE + 15,
    PREDICATE_PR63 = REG_PBASE + 63,
    PREDICATE_ALWAYS_TRUE = PREDICATE_PR0,  // PIN interface requirement
    PREDICATE_INVALID,
    PREDICATE_LAST
}PREDICATE_IPF;

                                                                  /* DO NOT EDIT */
extern string CATEGORY_StringShort(UINT32 cat);

                                                                  /* DO NOT EDIT */
extern UINT32 CFM_Sof(UINT64 cfm);

                                                                  /* DO NOT EDIT */
extern UINT32 CFM_Sol(UINT64 cfm);

                                                                  /* DO NOT EDIT */
extern UINT32 CFM_Sor(UINT64 cfm);

                                                                  /* DO NOT EDIT */
extern ADDRINT BSP_AddSlots(ADDRINT bsp, UINT32 slots);

                                                                  /* DO NOT EDIT */
extern ADDRINT BSP_SubSlots(ADDRINT bsp, UINT32 slots);

                                                                  /* DO NOT EDIT */
extern ADDRINT * REG_RseBackingStoreAddress(REG reg, ADDRINT * bsp);

                                                                  /* DO NOT EDIT */
extern VOID * FunctionCodeAddress(void (*fp)());

                                                                  /* DO NOT EDIT */
extern VOID * FunctionGp(void (*fp)());

                                                                  /* DO NOT EDIT */

