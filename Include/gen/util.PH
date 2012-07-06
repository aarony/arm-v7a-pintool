//Groups: @ingroup\s+(API_REF|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_ARM|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_ARM|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|PIN_MEMORY|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|OPTIMIZATION_API|MISC|CHECKPOINTING|INST_ARGS|PIN_THREAD_API|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_ARM|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


extern VOID SetAddress0x(BOOL val);

                                                                  /* DO NOT EDIT */
extern BOOL CharIsSpace(CHAR c);

                                                                  /* DO NOT EDIT */
extern CHAR CharToUpper(CHAR c);

                                                                  /* DO NOT EDIT */
extern string ptrstr(const VOID *val );

                                                                  /* DO NOT EDIT */
extern string rjstr( const string& s, UINT32 width, CHAR padding);

                                                                  /* DO NOT EDIT */
extern string StringFromAddrint( ADDRINT l);

                                                                  /* DO NOT EDIT */
extern string StringDec( UINT64 l, UINT32 digits, CHAR padding);

                                                                  /* DO NOT EDIT */
extern string StringDecSigned( INT64 l, UINT32 digits, CHAR padding);

                                                                  /* DO NOT EDIT */
extern string StringBignum( INT64 l, UINT32 digits, CHAR padding);

                                                                  /* DO NOT EDIT */
extern string Reformat(const string& s, const string& prefix, UINT32 min_line, UINT32 max_line);

                                                                  /* DO NOT EDIT */
extern string StringHex32( UINT32 l, UINT32 digits, BOOL prefix_0x);

                                                                  /* DO NOT EDIT */
extern string StringFlt(FLT64 val , UINT32 precision, UINT32 width);

                                                                  /* DO NOT EDIT */
extern string StringBool(BOOL b);

                                                                  /* DO NOT EDIT */
extern UINT32 Uint32FromString(const string& s);

                                                                  /* DO NOT EDIT */
extern UINT64 Uint64FromString(const string& s);

                                                                  /* DO NOT EDIT */
extern FLT64 FLT64FromString(const string& s);

                                                                  /* DO NOT EDIT */
extern ADDRINT AddrintFromString(const string& str);

                                                                  /* DO NOT EDIT */
extern UINT32 BitCount(UINT32 val);

                                                                  /* DO NOT EDIT */
extern string ReadLine(istream& inputFile, UINT32 *lineNum);

                                                                  /* DO NOT EDIT */
extern UINT32 Tokenize(const string& line, string *array, UINT32 n);

                                                                  /* DO NOT EDIT */
static inline string StringHex( UINT32 l, UINT32 digits, BOOL prefix_0x = TRUE) {return StringHex32(l, digits, prefix_0x);}

                                                                  /* DO NOT EDIT */
static inline string decstr(INT64 val, UINT32 width=0 )  {return StringDecSigned(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string decstr(INT32 val, UINT32 width=0 )  {return StringDecSigned(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string decstr(INT16 val, UINT32 width=0 )  {return StringDecSigned(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string decstr(UINT64 val, UINT32 width=0 )  {return StringDec(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string decstr(UINT32 val, UINT32 width=0 )  {return StringDec(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string decstr(UINT16 val, UINT32 width=0 )  {return StringDec(val,width,' ');}

                                                                  /* DO NOT EDIT */
static inline string hexstr(INT64 val, UINT32 width=0 )
{
    string ostr;
#if defined(_MSC_VER) && _MSC_VER  >= 1400
    ostr = StringHex( INT32((val >> 16) >> 16), width);
#else
    ostr = StringHex( INT32(val >> 32), width);
#endif
    ostr += StringHex( UINT32(val), 8, FALSE);

    return ostr;
}

                                                                  /* DO NOT EDIT */
static inline string hexstr(INT32 val, UINT32 width=0 )  {return StringHex(INT32(val),width);}

                                                                  /* DO NOT EDIT */
static inline string hexstr(INT16 val, UINT32 width=0 )  {return StringHex(INT32(val),width);}

                                                                  /* DO NOT EDIT */
static inline string hexstr(UINT64 val, UINT32 width=0 )
{
    string ostr;
#if defined(_MSC_VER) && _MSC_VER  >= 1400
    ostr = StringHex( UINT32((val >> 16) >> 16), width);
#else
    ostr = StringHex( UINT32(val >> 32), width);
#endif
    ostr += StringHex( UINT32(val), 8, FALSE);

    return ostr;
}

                                                                  /* DO NOT EDIT */
static inline string hexstr(VOID *p, UINT32 width=0 )
{
#if defined(HOST_IPF) || defined(HOST_IA32E)
    UINT64 val = reinterpret_cast<UINT64>(p);
    return hexstr(val,width);
#else
    UINT32 val = reinterpret_cast<UINT32>(p);
    return StringHex(val,width);
#endif    
}

                                                                  /* DO NOT EDIT */
static inline string hexstr(const VOID *p, UINT32 width=0 )
{
#if defined(HOST_IPF) || defined(HOST_IA32E)
    UINT64 val = reinterpret_cast<UINT64>(p);
    return hexstr(val,width);
#else
    UINT32 val = reinterpret_cast<UINT32>(p);
    return StringHex(val,width);
#endif    
}

                                                                  /* DO NOT EDIT */
static inline string hexstr(UINT32 val, UINT32 width=0 )  {return StringHex(UINT32(val),width);}

                                                                  /* DO NOT EDIT */
static inline string hexstr(UINT16 val, UINT32 width=0 )  {return StringHex(UINT32(val),width);}

                                                                  /* DO NOT EDIT */
static inline string fltstr(FLT64 val, UINT32 prec=0,UINT32 width=0 )  {return StringFlt(val,prec,width);}

                                                                  /* DO NOT EDIT */
static inline string ljstr(const string& s, UINT32 width, CHAR padding = ' ')
{
    string  ostr(width,padding);
    ostr.replace(0,s.length(),s);
    return ostr;
}

                                                                  /* DO NOT EDIT */
static inline VOID * Addrint2VoidStar(ADDRINT addr)
{
#if (defined(HOST_IA32E) || defined(HOST_IPF)) && (defined(TARGET_IA32) || defined(TARGET_ARM))
    ASSERT(false,"Should not be called in cross environment\n");
    return 0;
#else
    return reinterpret_cast<VOID*>(addr);
#endif    
}

                                                                  /* DO NOT EDIT */
static inline ADDRINT VoidStar2Addrint(const VOID * addr)
{
#if (defined(HOST_IA32E) || defined(HOST_IPF)) && (defined(TARGET_IA32) || defined(TARGET_ARM))
    ASSERT(false,"Should not be called in cross environment\n");
    return 0;
#else
    return reinterpret_cast<ADDRINT>(addr);
#endif
}

                                                                  /* DO NOT EDIT */
static inline ADDRINT VoidStar2Addrint(VOID * addr)
{
#if (defined(HOST_IA32E) || defined(HOST_IPF)) && (defined(TARGET_IA32) || defined(TARGET_ARM))
    ASSERT(false,"Should not be called in cross environment\n");
    return 0;
#else
    return reinterpret_cast<ADDRINT>(addr);
#endif
}

                                                                  /* DO NOT EDIT */
static inline ADDRINT RoundUp(ADDRINT value, UINT32 alignment)
{
    if (alignment == 0)
    {
        return value;
    }
    
    value += alignment - 1;
    value /= alignment;
    value *= alignment;

    return value;
}

                                                                  /* DO NOT EDIT */
static inline ADDRINT RoundDown(ADDRINT value, UINT32 alignment)
{
    if (alignment == 0)
    {
        return value;
    }

    value /= alignment;
    value *= alignment;

    return value;
}

                                                                  /* DO NOT EDIT */
static inline VOID * RoundUp(VOID * address, UINT32 alignment)
{
    return Addrint2VoidStar(RoundUp(VoidStar2Addrint(address), alignment));
}

                                                                  /* DO NOT EDIT */
static inline VOID * RoundDown(VOID * address, UINT32 alignment)
{
    return Addrint2VoidStar(RoundDown(VoidStar2Addrint(address), alignment));
}

                                                                  /* DO NOT EDIT */
template<typename T> ADDRINT PointerToInt(const T *ptr)
{
#if (defined(HOST_IA32) && !defined(TARGET_IA32)) || (defined(HOST_IA32E) && !defined(TARGET_IA32E)) || \
    (defined(HOST_IPF) && !defined(TARGET_IPF)) || (defined(HOST_ARM) && !defined(TARGET_ARM)) || \
    (defined(HOST_P14) && !defined(TARGET_P14))

    ASSERT(false,"Should not be called in cross environment\n");
    return 0;

#else
    return reinterpret_cast<ADDRINT>(ptr);
#endif
}

                                                                  /* DO NOT EDIT */
template<typename T> T *IntToPointer(ADDRINT addr)
{
#if (defined(HOST_IA32) && !defined(TARGET_IA32)) || (defined(HOST_IA32E) && !defined(TARGET_IA32E)) || \
    (defined(HOST_IPF) && !defined(TARGET_IPF)) || (defined(HOST_ARM) && !defined(TARGET_ARM)) || \
    (defined(HOST_P14) && !defined(TARGET_P14))

    ASSERT(false,"Should not be called in cross environment\n");
    return 0;

#else
    return reinterpret_cast<T*>(addr);
#endif
}

                                                                  /* DO NOT EDIT */
extern int VoidStar2Int(VOID * addr);

                                                                  /* DO NOT EDIT */
extern void RangeCheck(const UINT32 value, const UINT32 bits);

                                                                  /* DO NOT EDIT */
extern BOOL UnsignedImmediateFits(const UINT64 value, const UINT32 bits);

                                                                  /* DO NOT EDIT */
extern BOOL SignedImmediateFits(const INT64 value, const UINT32 origSize, const UINT32 extendedSize);

                                                                  /* DO NOT EDIT */
extern BOOL SignedImmediateFits(const INT64 value, const UINT32 origSize);

                                                                  /* DO NOT EDIT */
extern BOOL ExtractArgumentsPin(INT32 argc, CHAR **argv, INT32 * pargc, CHAR ***pargv);

                                                                  /* DO NOT EDIT */
extern BOOL ExtractArgumentsTool(INT32 argc, CHAR **argv, INT32 * targc, CHAR ***targv);

                                                                  /* DO NOT EDIT */
extern VOID GetArgumentsApp(INT32 * aargc, CHAR ***aargv);

                                                                  /* DO NOT EDIT */
extern BOOL ExtractArgumentsApp(INT32 argc, CHAR ** argv, INT32 * aargc, CHAR ***aargv);

                                                                  /* DO NOT EDIT */
extern INT32 MEMORY_ReadInt32(ADDRINT  address);

                                                                  /* DO NOT EDIT */
extern VOID MEMORY_WriteInt32(ADDRINT  address, INT32 value);

                                                                  /* DO NOT EDIT */
extern UINT32 MEMORY_ReadUint32(ADDRINT  address);

                                                                  /* DO NOT EDIT */
extern VOID MEMORY_WriteUint32(ADDRINT  address, UINT32 value);

                                                                  /* DO NOT EDIT */
extern INT64 MEMORY_ReadInt64(ADDRINT  address);

                                                                  /* DO NOT EDIT */
extern VOID MEMORY_WriteInt64(ADDRINT  address, INT64 value);

                                                                  /* DO NOT EDIT */
extern UINT64 MEMORY_ReadUint64(ADDRINT  address);

                                                                  /* DO NOT EDIT */
extern VOID MEMORY_WriteUint64(ADDRINT  address, UINT32 value);

                                                                  /* DO NOT EDIT */
extern USIZE RoundUpToPage(USIZE size);

                                                                  /* DO NOT EDIT */
extern "C" UINT64 ReadProcessorCycleCounter();

                                                                  /* DO NOT EDIT */
extern FLT64 ReadProcessorFrequency();

                                                                  /* DO NOT EDIT */
extern ADDRINT ProgramEntry(const ImageLoaderInfo * mainImage, const ImageLoaderInfo * loader);

                                                                  /* DO NOT EDIT */
extern CHAR* CreateTmpFileName(CHAR* fnameTemplate);

                                                                  /* DO NOT EDIT */

