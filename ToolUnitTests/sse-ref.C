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
#include "pin.H"
#include "xed-iclass.H"
#include <iostream>
#include <iomanip>

using namespace XED;
UINT64 icount = 0;

VOID mmx_arg(PIN_REGISTER* r, UINT32 opnd_indx, UINT32 regno)
{ 
    char buffer[512+16];
    char* aligned_bufp =reinterpret_cast<char*>(((reinterpret_cast<ADDRINT>(buffer) + 16) >> 4)<<4);
#if defined(PIN_GNU_COMPATIBLE)
    asm("fxsave %0" : "=m"(*aligned_bufp));
#else
    __asm
    {
        push eax
        mov  eax, aligned_bufp
        fxsave [eax]
        pop eax
    }
#endif

#if defined(DEBUG_SSE_REF)
    cout << "MMX" << regno << " operand_index: " << opnd_indx << " ";
    cout << setw(10) << r->dword[0] << " ";
    cout << setw(10) << r->dword[1] << " ";
    cout << endl;
#endif
    // increment the destination...
    if (opnd_indx == 0)
    {
        r->dword[0] ++;
    }
#if defined(PIN_GNU_COMPATIBLE)
    asm volatile("fxrstor %0" :: "m"(*aligned_bufp));
#else
    __asm
    {
	    push eax
        mov  eax, aligned_bufp 
        fxrstor [eax]
        pop eax
    }
#endif
    icount++; 
}

VOID xmm_arg(PIN_REGISTER* r, UINT32 opnd_indx, UINT32 regno)
{ 
    char buffer[512+16];
    char* aligned_bufp =reinterpret_cast<char*>(((reinterpret_cast<ADDRINT>(buffer) + 16) >> 4)<<4);
#if defined(PIN_GNU_COMPATIBLE)
    asm("fxsave %0" : "=m"(*aligned_bufp));
#else
    __asm
    {
        push eax
        mov  eax, aligned_bufp
        fxsave [eax]
        pop eax
    }
#endif

#if defined(DEBUG_SSE_REF)
    cout << "XMM" << regno << " operand_index: " << opnd_indx << " ";
    for(unsigned int i=0;i< MAX_DWORDS_PER_PIN_REG;i++)
    {
        cout << setw(10) << r->dword[i] << " ";
    }
    cout << endl;
#endif
    // increment the destination...
    if (opnd_indx == 0)
    {
        r->dword[0] ++;
    }
#if defined(PIN_GNU_COMPATIBLE)
    asm volatile("fxrstor %0" :: "m"(*aligned_bufp));
#else
    __asm
    {
	    push eax
        mov  eax, aligned_bufp 
        fxrstor [eax]
        pop eax
    }
#endif

    icount++; 
}
    
VOID Instruction(INS ins, VOID *v)
{
    xed_iclass_t iclass = (xed_iclass_t)INS_Opcode(ins);
    if (iclass == XEDICLASS_MOVQ || iclass == XEDICLASS_MOVDQU)
    {
        const unsigned int opnd_count =  INS_OperandCount(ins);
        unsigned int i=0;
        //for(unsigned int i=0; i < opnd_count;i++)
        {
            if (INS_OperandIsReg(ins,i))
            {
                REG r = INS_OperandReg(ins,i);
                if (REG_is_mm(r))
                {
                    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)mmx_arg, 
                                   IARG_REG_REFERENCE,
                                   r,
                                   IARG_UINT32,
                                   i,
                                   IARG_UINT32, 
                                   (r-REG_MM_BASE),

                                   IARG_END);
                }
                if (REG_is_xmm(r))
                {
                    INS_InsertCall(ins, IPOINT_AFTER, (AFUNPTR)xmm_arg, 
                                   IARG_REG_REFERENCE,
                                   r,
                                   IARG_UINT32,
                                   i,
                                   IARG_UINT32, 
                                   (r-REG_XMM_BASE),
                                   IARG_END);
                }
                
            }
        }
    }

}

VOID Fini(INT32 code, VOID *v)
{
    // Don't output icount as part of the reference output
    // because the dynamic loader may also use xmm insts.
    
    //std::cerr << "Count: " << icount << endl;
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
