	.file	"heapsort.c"
	.text
	.p2align 4,,15
	.globl	heapsort
	.type	heapsort, @function
heapsort:
.LFB11:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ebx
	call	heapify_array
	testl	%ebx, %ebx
	js	.L1
	.p2align 4,,10
	.p2align 3
.L7:
	movl	%ebx, %edi
	call	extract_max
	movslq	%ebx, %rdx
	subl	$1, %ebx
	cmpl	$-1, %ebx
	movl	%eax, heap(,%rdx,4)
	jne	.L7
.L1:
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE11:
	.size	heapsort, .-heapsort
	.ident	"GCC: (SUSE Linux) 4.8.5"
	.section	.note.GNU-stack,"",@progbits
