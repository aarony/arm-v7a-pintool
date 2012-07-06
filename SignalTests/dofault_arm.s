/*
 * void DoSIGSEGV()
 */
.text
    .align 5
.globl DoSIGSEGV
DoSIGSEGV:
    mov     r0, #9
    ldr     r0, [r0]
    mov     pc, lr


/*
 * void DoSIGBUS()
 */
.globl DoSIGBUS
DoSIGBUS:
    // @todo: Don't know how to generate a SIGBUS on ARM
   mov pc, lr


/*
 * void DoSIGILL()
 */
.globl DoSIGILL
DoSIGILL:
.word 0xffffffff
    mov pc, lr


/*
 * void DoSIGFPE()
 */
.globl DoSIGFPE
DoSIGFPE:
   mov pc, lr


/*
 * void DoSIGTRAP()
 */
.globl DoSIGTRAP
DoSIGTRAP:
.word 0xef9f0001
    mov pc, lr
