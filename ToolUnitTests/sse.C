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
#include <iomanip>
using namespace std;
#define N 1024
int main(int argc, char** argv);
#include <stdint.h>
typedef uint8_t  UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint32_t UINT;
typedef uint64_t UINT64;

typedef int8_t  INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;


#define MAX_XMM_REGS 16
#define MAX_BYTES_PER_XMM_REG 16
#define MAX_WORDS_PER_XMM_REG (MAX_BYTES_PER_XMM_REG/2)
#define MAX_DWORDS_PER_XMM_REG (MAX_WORDS_PER_XMM_REG/2)
#define MAX_QWORDS_PER_XMM_REG (MAX_DWORDS_PER_XMM_REG/2)
#define MAX_FLOATS_PER_XMM_REG (MAX_BYTES_PER_XMM_REG/sizeof(float))
#define MAX_DOUBLES_PER_XMM_REG (MAX_BYTES_PER_XMM_REG/sizeof(double))

#define MAX_MMX_REGS 8
#define MAX_BYTES_PER_MMX_REG 8
#define MAX_WORDS_PER_MMX_REG (MAX_BYTES_PER_MMX_REG/2)
#define MAX_DWORDS_PER_MMX_REG (MAX_WORDS_PER_MMX_REG/2)
#define MAX_QWORDS_PER_MMX_REG (MAX_DWORDS_PER_MMX_REG/2)
#define MAX_FLOATS_PER_MMX_REG (MAX_BYTES_PER_MMX_REG/sizeof(float))
#define MAX_DOUBLES_PER_MMX_REG (MAX_BYTES_PER_MMX_REG/sizeof(double))


union xmm_reg_t
{
    UINT8  byte[MAX_BYTES_PER_XMM_REG];
    UINT16 word[MAX_WORDS_PER_XMM_REG];
    UINT32 dword[MAX_DWORDS_PER_XMM_REG];
    UINT64 qword[MAX_QWORDS_PER_XMM_REG];

    INT8   s_byte[MAX_BYTES_PER_XMM_REG];
    INT16  s_word[MAX_WORDS_PER_XMM_REG];
    INT32  s_dword[MAX_DWORDS_PER_XMM_REG];
    INT64  s_qword[MAX_QWORDS_PER_XMM_REG];

    float  flt[MAX_FLOATS_PER_XMM_REG];
    double dbl[MAX_DOUBLES_PER_XMM_REG];

} __attribute__ ((aligned(16)));

union mmx_reg_t
{
    UINT8  byte[MAX_BYTES_PER_MMX_REG];
    UINT16 word[MAX_WORDS_PER_MMX_REG];
    UINT32 dword[MAX_DWORDS_PER_MMX_REG];
    UINT64 qword[MAX_QWORDS_PER_MMX_REG];

    INT8  s_byte[MAX_BYTES_PER_MMX_REG];
    INT16 s_word[MAX_WORDS_PER_MMX_REG];
    INT32 s_dword[MAX_DWORDS_PER_MMX_REG];
    INT64 s_qword[MAX_QWORDS_PER_MMX_REG];

    float  flt[MAX_FLOATS_PER_MMX_REG];
    double dbl[MAX_DOUBLES_PER_MMX_REG];

} __attribute__ ((aligned(8)));

static void
set_xmm_reg0(xmm_reg_t& xmm_reg)
{
   asm volatile("movdqu %0, %%xmm0" :  : "m" (xmm_reg) : "%xmm0"  );
} 
static void
get_xmm_reg0(xmm_reg_t& xmm_reg)
{
   asm volatile("movdqu %%xmm0,%0" : "=m" (xmm_reg)  );
}

static void
set_mmx_reg0(mmx_reg_t& mmx_reg)
{
   asm volatile("movq %0, %%mm0" :  : "m" (mmx_reg) : "%mm0"  );
} 
static void
get_mmx_reg0(mmx_reg_t& mmx_reg)
{
   asm volatile("movq %%mm0,%0" : "=m" (mmx_reg)  );
}

UINT32 init_sse(UINT32 z)
{

    xmm_reg_t xmm;
    xmm.dword[0] = z;
    set_xmm_reg0(xmm); // from memory to register -- we modify the output using the tool
    get_xmm_reg0(xmm); // from register to memory
    return xmm.dword[0];
}

UINT32 init_mmx(UINT32 z)
{
    mmx_reg_t mmx;
    mmx.dword[0] = z;
    set_mmx_reg0(mmx);// from mem to register -- we modify the output of this one
    get_mmx_reg0(mmx); // from register to memory 
    return mmx.dword[0];
}


int main(int argc, char** argv)
{
    UINT32 x = init_sse(atoi(argv[1]));
    cout << x << endl;
    UINT32 y = init_mmx(x);
    cout << y << endl;
    return 0;
}
 
