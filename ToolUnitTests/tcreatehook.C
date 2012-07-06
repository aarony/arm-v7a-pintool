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

static char* replacedfunname = "__pthread_create_2_1";
typedef int (*REPLACEFUNTYPE)(pthread_t * thread, pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
static AFUNPTR orig_fptr = 0;

class CHANNEL
{
  public:
    THREAD_STARTROUTINE _startroutine;
    VOID* _arg;

    CHANNEL(): _startroutine(0), _arg(0) {}
    VOID Setup(THREAD_STARTROUTINE sr, VOID* a)
    {
        _startroutine = sr;
        _arg = a;
    }
};

//typedef void *(*STARTROUTINE_TYPE)(void *);

// Note: we should dynamically allocate and free channel, as shown below in the commented out "new channel" and "delete channel".
// However, calling "new" and "delete" will invoke RC's thread local
// storage support (when libpinpthread is used), which won't work here because
// my_pthread_create() and my_wrapper_run() are using the application stack, not the pin stack
// for they arenot jitted.

static CHANNEL channel;

void* my_wrapper_run(void* wrapper_arg)
{   
    printf("TOOL: Inside my_wrapper_run()\n");
    fflush(stdout);
    
    CHANNEL* channel = reinterpret_cast<CHANNEL*> (wrapper_arg);

    void* result = PIN_JitThreadStartRoutine(channel->_startroutine, channel->_arg);

#if 0
    delete channel; 
#endif
    
    printf("TOOL: after PIN_JitThreadStartRoutine()\n");
    fflush(stdout);
    
    return result;
}

int my_pthread_create(pthread_t * thread, pthread_attr_t * attr, void * (*start_routine)(void *), void * arg)
{
    printf("TOOL: Inside my_pthread_create()\n");
    fflush(stdout);

    REPLACEFUNTYPE fun = reinterpret_cast<REPLACEFUNTYPE>(orig_fptr);

    // channel must be allocated on the heap instead of on the stack frame on
    // my_pthread_create() since channel could be dereferenced in PIN_JitThreadStartRoutine()
    // after we return from my_pthread_create()

#if 1
    channel.Setup(start_routine, arg);
#else
    CHANNEL* channel = new CHANNEL(start_routine, arg);
    ASSERTX(channel);
#endif
    
    return fun(thread, attr, my_wrapper_run, &channel);
}

VOID ImageLoad(IMG img, VOID * v)
{
    //printf("TOOL: img %s is loaded\n", IMG_Name(img).c_str());
    //fflush(stdout);
    
    RTN rtn = RTN_FindByName(img, replacedfunname);
    
    if (RTN_Valid(rtn))
    {
        //printf("TOOL: Pthread create found\n");
        //fflush(stdout);
        
        RTN_ReplacedByNativeCallToFun(rtn, reinterpret_cast<VOID(*)()>(my_pthread_create));
        
        orig_fptr = RTN_Funptr(rtn);
    }
}

VOID Fini(INT32 code, VOID *)
{
    fprintf(stderr, "\nTOOL: Done\n");
}

int main(INT32 argc, CHAR **argv)
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
