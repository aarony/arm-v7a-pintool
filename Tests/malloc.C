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

KNOB<INT32> KnobMaxSize(KNOB_MODE_WRITEONCE, "pintool",
    "m", "0x6000000", "Total bytes to allocate");
KNOB<INT32> KnobIncrement(KNOB_MODE_WRITEONCE, "pintool",
    "i", "100", "Bytes to malloc each time");


VOID MalMalloc()
{
    for (INT32 size = 0; size < KnobMaxSize; size+=KnobIncrement)
    {
        VOID * m = malloc(KnobIncrement);
        if (m == 0)
        {
            fprintf(stderr, "Failed malloc\n");
        }
    }
}

int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);
    
    MalMalloc();
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
