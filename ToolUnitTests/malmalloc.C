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

char * data[100];
INT32 sizeindex[100];

INT32 sizes[] = 
{
    100, 4000, 30, 20, 6000, 24000, 0
};
               
VOID mal(INT32 id)
{
    char * d = data[id];
    INT32 size = sizes[sizeindex[id]];

    if (d)
    {
        for (INT32 i = 0; i < size; i++)
        {
            if (d[i] != id)
            {
                fprintf(stderr,"Bad data id %d data %d\n", id, d[i]);
                exit(1);
            }
        }
        free(d);
    }

    sizeindex[id]++;

    if (sizes[sizeindex[id]] == 0)
        sizeindex[id] = 0;
    size = sizes[sizeindex[id]];

    ASSERTX(size != 0);
    
    
    data[id] = (char*)malloc(size);
    d = data[id];
    for (INT32 i = 0; i < size; i++)
    {
        d[i] = id;
    }
}

VOID Tr(TRACE trace, VOID *)
{
    TRACE_InsertCall(trace, IPOINT_BEFORE, AFUNPTR(mal), IARG_THREAD_ID, IARG_END);
}

int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);
    
    TRACE_AddInstrumentFunction(Tr, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
