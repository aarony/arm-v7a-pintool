/*
 * void DoSIGSEGV()
 */
    .text
    .align 16
    .global DoSIGSEGV
    .proc DoSIGSEGV
DoSIGSEGV:
    .prologue
    .body
    mov r14 = 9;;
    st8 [r14] = r0
    br.ret.sptk.many b0;;
    .endp DoSIGSEGV


/*
 * void DoSIGBUS()
 */
    .text
    .align 16
    .global DoSIGBUS
    .proc DoSIGBUS
DoSIGBUS:
    .prologue
    .body

    // Set UM.ac (alignment check).  This should force an unaligned access to fault.
    //
    mov r14 = psr.um;;
    or r14 = 0x8, r14;;
    mov psr.um = r14;;

    movl r14 = Var+1;;
    st8 [r14] = r14         // Do an unaligned access
    br.ret.sptk.many b0;;
    .endp DoSIGBUS


    .data
    .align 8
Var:
    .long 0


/*
 * void DoSIGILL()
 */
    .text
    .align 16
    .global DoSIGILL
    .proc DoSIGILL
DoSIGILL:
    .prologue
    .body
    nop 0;;
    nop 1;;
    nop 2;;
    mov ar.bsp = r0;;   // This is an illegal instruction.
    nop 0;;
    nop 1;;
    nop 2;;
    br.ret.sptk.many b0;;
    .endp DoSIGILL


/*
 * void DoSIGFPE()
 */
    .text
    .align 16
    .global DoSIGFPE
    .proc DoSIGFPE
DoSIGFPE:
    .prologue
    .body

    // Clear all the trap bits in FPSR, which enables all FP exceptions.
    //
    mov r14 = ar.fpsr;;
    nop 0;;
    nop 1;;
    nop 2;;
    dep r14 = 0, r14, 0, 6;;
    nop 0;;
    nop 1;;
    nop 2;;
    mov ar.fpsr = r14;
    nop 0;;
    nop 1;;
    nop 2;;

    frcpa f6, p6 = f1, f0   // Divide by zero
    br.ret.sptk.many b0;;
    .endp DoSIGFPE


/*
 * void DoSIGTRAP()
 */
    .text
    .align 16
    .global DoSIGTRAP
    .proc DoSIGTRAP
DoSIGTRAP:
    .prologue
    .body
    break 0xccccc
    .endp DoSIGTRAP
