.syntax unified

.extern WriteHex32
.extern WriteLine
.extern toggleKernel
.extern isInKernel

.section .text.PendSV_Handler,"ax",%progbits
.align 2
.global PendSV_Handler
.thumb
.thumb_func
.type PendSV_Handler, %function
PendSV_Handler:
	.cfi_startproc
	//backup scratch regisers
	blx isInKernel
	cbz	r0, psv_backup_psp
psv_backup_msp:
	mrs r0, msp
	stmdb r0!, {r4-r11}
	msr msp, r0
	b psv_backup_done
psv_backup_psp:
	mrs	r0, psp
	stmdb r0!, {r4-r11}
	msr psp, r0
psv_backup_done:


	blx toggleKernel
	cbz	r0, psv_return_psp
psv_return_msp:
	//restore scratch registers
	mrs r0, msp
	ldmfd r0!, {r4-r11}
	msr msp, r0
	//sudo: mov lr, #0xfffffff9 --> Return in Kernel Mode
	mov r0, #0
	sub	r0, r0, #7
	mov lr, r0
	//b	psv_report_regs
	bx	lr

psv_return_psp:
	//restore scratch registers
	mrs r0, psp
	ldmfd r0!, {r4-r11}
	msr psp, r0
	//sudo: mov lr, #0xfffffffd --> Return in Thread Mode
	mov r0, #0
	sub	r0, r0, #3
	mov lr, r0
	//b	psv_report_regs
	bx 	lr

psv_report_regs:
	//temporary to test sp chanes in isr
	blx isInKernel
	cbz	r0, psv_report_psp
psv_report_msp:
	mrs	r0, msp
	b	psv_report_sp_done
psv_report_psp:
	mrs	r0, psp
psv_report_sp_done:
	blx	WriteHex32
	blx WriteLine
	//write stack values
	//r0
	ldr	r0, [sp, #0]
	blx WriteHex32
	blx WriteLine
	//r1
	ldr	r0, [sp, #4]
	blx WriteHex32
	blx WriteLine
	//r2
	ldr	r0, [sp, #8]
	blx WriteHex32
	blx WriteLine
	//r3
	ldr	r0, [sp, #12]
	blx WriteHex32
	blx WriteLine
	//r12
	ldr	r0, [sp, #16]
	blx WriteHex32
	blx WriteLine
	//lr
	ldr	r0, [sp, #20]
	blx WriteHex32
	blx WriteLine
	//pc
	ldr	r0, [sp, #24]
	blx WriteHex32
	blx WriteLine
	//xPSR
	ldr	r0, [sp, #28]
	blx WriteHex32
	blx WriteLine
	blx WriteLine
	b	psv_fin_ret

psv_fin_ret:
	blx isInKernel
	cbz	r0, psv_fin_ret_psp
psv_fin_ret_msp:
	mov r0, #0
	sub	r0, r0, #7
	mov lr, r0
	bx lr
psv_fin_ret_psp:
	mov r0, #0
	sub	r0, r0, #3
	mov lr, r0
	bx lr

	.cfi_endproc
	.size PendSV_Handler, .-PendSV_Handler

.end
