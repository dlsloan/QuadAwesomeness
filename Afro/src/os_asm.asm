.syntax unified

.extern _toggleKernel
.extern _isInKernel

.section .text.PendSV_Handler,"ax",%progbits
.align 2
.global PendSV_Handler
.thumb
.thumb_func
.type PendSV_Handler, %function
PendSV_Handler:
	.cfi_startproc
	//backup scratch regisers
	blx _isInKernel
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

	blx _toggleKernel
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
	bx 	lr

	.cfi_endproc
	.size PendSV_Handler, .-PendSV_Handler

.end
