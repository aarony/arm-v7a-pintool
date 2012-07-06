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
#include <iostream>
#include "pin.H"
#include <map>

using namespace std;

#if defined(TARGET_MAC)
static char* replacedfunname = "_addnums";
#else
static char* replacedfunname = "addnums";
#endif

typedef int (*REPLACEFUNTYPE)(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9);

class NATIVE_FUN_ID
{
  public:
    const CHAR* _funname;

    NATIVE_FUN_ID(const CHAR* s): _funname(s) {}
    bool operator == (const NATIVE_FUN_ID& other) const
    {
        return strcmp(_funname, other._funname)==0;
    }
    bool operator < (const NATIVE_FUN_ID& other) const
    {
        return strlen(_funname) < strlen(other._funname);
    }
};

typedef map<NATIVE_FUN_ID, AFUNPTR> NATIVE_FUN_MAP;

static NATIVE_FUN_MAP native_fun_map;

int replacement_addnums(int n0, int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9)
{
    static bool first = TRUE;
    static REPLACEFUNTYPE fun;
    
    
    if (first)
    {   // find from the map only the first time. After that, we use the saved result
        
        first = FALSE;
        NATIVE_FUN_MAP::const_iterator result = native_fun_map.find(NATIVE_FUN_ID(replacedfunname));
        ASSERTX(result != native_fun_map.end());
        fun = reinterpret_cast<REPLACEFUNTYPE>(result->second);
    }
    
    printf("Inside replacement_addnums()\n");
    fflush(stdout);
    
    return fun(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9);

}

VOID itc_callback_push(AFUNPTR replacedFun)
{
    //printf("itc_callback_push() is called with replacedFun= %p\n", replacedFun);
    printf("itc_callback_push() is called\n");
    fflush(stdout);
}

VOID ImageLoad(IMG img, VOID * v)
{
    RTN rtn = RTN_FindByName(img, replacedfunname);
    
    if (RTN_Valid(rtn))
    {
        // Complex replacememt will call itc_callback_push before calling the replaceent function;
        // simple replacement (RTN_ReplaceWithUninstrumentedRoutine()) won't
        
        RTN_ComplexReplaceWithUninstrumentedRoutine(rtn, reinterpret_cast<VOID(*)()>(replacement_addnums));
        
        const AFUNPTR fptr = RTN_Funptr(rtn);

        native_fun_map[NATIVE_FUN_ID(RTN_Name(rtn).c_str())] = fptr;
    }
}

VOID Fini(INT32 code, VOID *)
{
    fprintf(stderr, "\nDone\n");
}

int main(INT32 argc, CHAR **argv)
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    // register itc_callback_push with Pin
    PIN_RegisterItcAuxCallBackPushFun(reinterpret_cast<AFUNPTR>(itc_callback_push));
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
