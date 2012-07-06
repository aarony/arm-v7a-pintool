/*
 * void DoSIGSEGV()
 */
.text
    .align 4
.globl DoSIGSEGV
DoSIGSEGV:
    mov     $0x9, %rax
    mov     0(%rax), %rax
    ret


/*
 * void DoSIGBUS()
 */
.text
    .align 4
.globl DoSIGBUS
DoSIGBUS:
    // @todo: Don't know how to generate a SIGBUS on IA32e
    ret


/*
 * void DoSIGILL()
 */
.text
    .align 4
.globl DoSIGILL
DoSIGILL:
    ud2
    ret


/*
 * void DoSIGFPE()
 */
.text
    .align 4
.globl DoSIGFPE
DoSIGFPE:
    mov     $0x0, %rax
    idiv    %rax
    ret


/*
 * void DoSIGTRAP()
 */
.text
    .align 4
.globl DoSIGTRAP
DoSIGTRAP:
    int     $3
    ret
