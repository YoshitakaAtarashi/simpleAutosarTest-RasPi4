/*
 * Boot code for Raspberry Pi 4 (AArch32 mode)
 * This is a minimal boot stub for bare metal applications.
 *
 * Raspberry Pi 4 (BCM2711) boots all cores in AArch32 HYP mode (EL2).
 * We must switch to SVC mode (EL1) before running application code.
 */

.section ".text.boot"

.global _start

_start:
    /* Get CPU ID - only CPU 0 should continue */
    mrc p15, 0, r0, c0, c0, 5
    and r0, r0, #3
    cmp r0, #0
    bne halt

    /*
     * Raspberry Pi 4 boots in AArch32 HYP mode (EL2).
     * Switch to SVC mode (EL1) using ERET from HYP.
     */
    mrs r1, cpsr                /* Save full CPSR for later reuse */
    and r0, r1, #0x1F           /* Extract current mode bits */
    cmp r0, #0x1A               /* 0x1A = HYP mode */
    bne _skip_hyp_exit

    /* Build SPSR for SVC mode with IRQ/FIQ disabled */
    bic r0, r1, #0x1F           /* Clear mode bits (reuse saved CPSR) */
    orr r0, r0, #0x13           /* SVC mode */
    orr r0, r0, #0xC0           /* Disable IRQ and FIQ */
    msr spsr_cxsf, r0
    /* Set ELR_hyp to the instruction after the eret */
    adr r0, _skip_hyp_exit
    msr elr_hyp, r0
    eret                        /* Switch to SVC mode */

_skip_hyp_exit:
    /* Set stack pointer */
    ldr sp, =__stack_end

    /* Clear BSS section */
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0

clear_bss:
    cmp r0, r1
    bge clear_bss_done
    str r2, [r0], #4
    b clear_bss

clear_bss_done:
    /* Enable VFP/NEON (if needed) */
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #0x300000    /* Single precision */
    orr r0, r0, #0xC00000    /* Double precision */
    mcr p15, 0, r0, c1, c0, 2
    isb
    mov r0, #0x40000000
    vmsr fpexc, r0

    /* Jump to C main function */
    bl main

halt:
    wfe
    b halt

.section ".data"
/* Data section */

.section ".bss"
/* BSS section */
