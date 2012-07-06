//Groups: @ingroup\s+(API_REF|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_ARM|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_ARM|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|PIN_MEMORY|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|OPTIMIZATION_API|MISC|CHECKPOINTING|INST_ARGS|PIN_THREAD_API|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_ARM|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


typedef enum
{
    REG_INVALID_ = 0,
    REG_NONE = 1,
    REG_FIRST = 2,
    
    REG_IMMBASE = REG_FIRST,
    REG_IMM_MASK = REG_IMMBASE + 1,
    REG_IMM_LLIT = REG_IMMBASE + 2,
    REG_IMM_SLIT = REG_IMMBASE + 3,
    REG_IMM_FPLIT = REG_IMMBASE + 4,

    REG_MULTIPLE,

    REG_CPSR = 14,
    REG_SPSR = 15,
    REG_RBASE = 16, /* 16 general registers */

    REG_MACHINE_BASE = REG_CPSR, /* real registers on the machine */
    REG_APPLICATION_BASE = REG_CPSR, /* register names used by the application */
    REG_PHYSICAL_CONTEXT_BEGIN = REG_CPSR,

    REG_GR_BASE = REG_RBASE, /* general register base */
    
    REG_R0 = REG_RBASE + 0,
    REG_R1 = REG_RBASE + 1,
    REG_R2 = REG_RBASE + 2,
    REG_R3 = REG_RBASE + 3,
    REG_R4 = REG_RBASE + 4,
    REG_R5 = REG_RBASE + 5,
    REG_R6 = REG_RBASE + 6,
    REG_R7 = REG_RBASE + 7,
    REG_R8 = REG_RBASE + 8,
    REG_R9 = REG_RBASE + 9,
    REG_R10 = REG_RBASE + 10,
    REG_R11 = REG_RBASE + 11,
    REG_R12 = REG_RBASE + 12,

    REG_R13 = REG_RBASE + 13,
    REG_STACK_PTR = REG_RBASE + 13, // generic, isa-independent stack pointer
    
    REG_R14 = REG_RBASE + 14,
    REG_LR = REG_RBASE + 14,

    REG_R15 = REG_RBASE + 15,
    REG_PC = REG_RBASE + 15,
    REG_INST_PTR = REG_PC, // generic, isa-independent inst pointer
    REG_PHYSICAL_CONTEXT_END = REG_PC,

    REG_FBASE = 32, /* FPA coprocessor: 8 fp + control + status register */
    
    REG_F0 = REG_FBASE + 0,
    REG_F1 = REG_FBASE + 1,
    REG_F2 = REG_FBASE + 2,
    REG_F3 = REG_FBASE + 3,
    REG_F4 = REG_FBASE + 4,
    REG_F5 = REG_FBASE + 5,
    REG_F6 = REG_FBASE + 6,
    REG_F7 = REG_FBASE + 7,

    REG_FPCR,
    REG_FPSR,
    /* gap in number space! */
    
    REG_CBASE = 48, /* 16 coprocessor register */
    
    REG_C0 = REG_CBASE + 0,
    REG_C15 = REG_CBASE + 15,

    REG_DBASE = 64, /* 16 double vfp registers */

    REG_D0 = REG_DBASE + 0,
    REG_D15 = REG_DBASE + 15,

    REG_SBASE = 80, /* 32 single vfp registers */
    
    REG_S0 = REG_SBASE + 0,
    REG_S31 = REG_SBASE + 31,

    REG_MACHINE_LAST =  REG_S31, /* last machine register */
    
    REG_NONAGGREGATE_LAST = REG_S31,

    REG_APPLICATION_LAST = REG_S31,/* last register name used by the application */

    /* Pin's virtual register names */
    REG_PIN_BASE,

    // ISA-dependent Pin virtual regs (nothing for now)
    REG_PIN_GR_BASE = REG_PIN_BASE,
    
    // ISA-independent Pin virtual regs needed for instrumentation
    REG_INST_BASE,
    REG_INST_G0 = REG_INST_BASE,
    REG_INST_G1,
    REG_INST_G2,
    REG_INST_G3,
    REG_INST_G4,
    REG_INST_G5,
    REG_INST_G6,
    REG_INST_G7,
    REG_INST_G8,
    REG_INST_G9,
    REG_INST_LAST = REG_INST_G9,
    
    REG_THREAD_ID,
    REG_PIN_STACK_PTR, // Pointer to the Pin stack
    REG_PIN_INDIRREG, /* virtual reg holding indirect jmp target value */
    REG_INST_COND, // virtual register used in conditional instrumentation
    REG_PIN_SPILLPTR, /* ptr to the pin spill area */
    REG_PIN_GR_LAST=REG_PIN_SPILLPTR,
    REG_PIN_LAST = REG_PIN_GR_LAST,
    
    REG_LAST
} REG;

                                                                  /* DO NOT EDIT */
const ADDRINT NUM_PHYSICAL_REGS = REG_MACHINE_LAST - REG_MACHINE_BASE + 1;

                                                                  /* DO NOT EDIT */
const ADDRINT NUM_SPILLED_REGS =  NUM_PHYSICAL_REGS + (REG_PIN_LAST - REG_PIN_BASE + 1);

                                                                  /* DO NOT EDIT */
const ADDRINT NUM_FP_REGS = REG_F7 - REG_FBASE + 1;

                                                                  /* DO NOT EDIT */
static inline BOOL REG_is_gr(REG reg){ return (reg >= REG_RBASE) && (reg <= REG_RBASE+15);}

                                                                  /* DO NOT EDIT */
static inline BOOL REG_is_seg(REG reg)  { return FALSE; }

                                                                  /* DO NOT EDIT */
static inline BOOL REG_is_br(REG reg)  { return FALSE; }

                                                                  /* DO NOT EDIT */
typedef enum
{
    REGNAME_FIRST = 0, /* index into array, must start at 0 */
    REGNAME_RN = REGNAME_FIRST,
    REGNAME_RDHI = REGNAME_RN,  /* multiply INSs */
    REGNAME_RD,
    REGNAME_RDLO = REGNAME_RD,  /* multiply INSs */
    REGNAME_RS,
    REGNAME_RM,
    REGNAME_LAST = REGNAME_RM
}REGNAME;

                                                                  /* DO NOT EDIT */
extern string REG_StringShort(REG reg);

                                                                  /* DO NOT EDIT */
extern REG REG_FullRegName(const REG reg);

                                                                  /* DO NOT EDIT */

