#
# Calling conventions:
#     %rdi, %rsi, %rdx, %rcx, %r8, %r9, %r10, %r11 and %rax are caller saved
#     %rbx, %rbp, %r12, %r13, %r14, %r15 are callee saved
#     %rsp can not be used except for its normal use as a stack pointer.
#     argument are passed through registers %rdi, %rsi, %rdx in this order.
#     values are returned through %rax
#
.pos 0x100

main:	irmovq bottom,  %rsp     # initialize stack

	xorq   %rdi, %rdi        # %rdi = 0
	mrmovq size(%rdi), %rdi  # %rdi = size
	irmovq $1, %rsi
	subq   %rsi, %rdi        # %rdi = size - 1
	
	call   heapsort
	halt

#
# Swap:
#    %rdi: index1
#    %rsi: index2
#
swap:	addq   %rdi, %rdi	# Offset is 8 times the index
	addq   %rdi, %rdi
	addq   %rdi, %rdi
	addq   %rsi, %rsi	# Offset is 8 times the index
	addq   %rsi, %rsi
	addq   %rsi, %rsi
	
	mrmovq heap(%rdi), %rcx # tmp = heap[index1]
	mrmovq heap(%rsi), %rdx # heap[index1] = heap[index2]
	rmmovq %rdx, heap(%rdi)
	rmmovq %rcx, heap(%rsi) # heap[index2] = tmp

	ret

#
# Check_child
#     %rdi: child index
#     %rsi: index of highest
#     %rdx: last
# Returns:
#     %rax: true if child <= last && heap[highest] < heap[child]
#
check_child:
	pushq  %rbx
	xorq   %rax, %rax       # set return value to false.

	rrmovq %rdi, %rbx       # if child > last, skip
	subq   %rdx, %rbx
	jg     check_child_finish

	rrmovq %rsi, %rbx       # %rbx = heap[highest]
	addq   %rbx, %rbx
	addq   %rbx, %rbx
	addq   %rbx, %rbx
	mrmovq heap(%rbx), %rbx

	rrmovq %rdi, %rcx       # %rcx = heap[child]
	addq   %rcx, %rcx
	addq   %rcx, %rcx
	addq   %rcx, %rcx
	mrmovq heap(%rcx), %rcx

	irmovq $1,   %r8
	subq   %rbx, %rcx      # if heap[child] > heap[highest], return 1
	cmovg  %r8,  %rax

check_child_finish:
	popq %rbx
	ret

#
# Heapify_node
#     %rdi: index
#     %rsi: last
#
# Local variables:
#     %rbx: index
#     %r12: highest
#     %r13: last
#
heapify_node:
	pushq  %rbx             # Save %rbx and use it to store index
	rrmovq %rdi, %rbx
	pushq  %r12
	rrmovq %rdi, %r12       # %r12 = highest
	pushq  %r13
	rrmovq %rsi, %r13       # %r13 = last

heapify_loop:
	rrmovq %rbx, %rdi       # left_child = 2 * index + 1
	addq   %rdi, %rdi
	irmovq $1,   %r8
	addq   %r8,  %rdi
	rrmovq %r12, %rsi
	rrmovq %r13, %rdx

	call   check_child
	andq   %rax, %rax
	cmovne %rdi, %r12       # highest = left_child (if condition ok)

	rrmovq %rbx, %rdi       # right_child = 2 * index + 2
	addq   %rdi, %rdi
	irmovq $2,   %r8	
	addq   %r8,  %rdi
	rrmovq %r12, %rsi
	rrmovq %r13, %rdx

	call   check_child
	andq   %rax, %rax
	cmovne %rdi, %r12       # highest = right_child (if condition ok)

heapify_skip2:
	rrmovq %r12, %rdi
	subq   %rbx, %rdi
	je     heapify_finish

	rrmovq %r12, %rdi
	rrmovq %rbx, %rsi
	call   swap

	rrmovq %r12, %rbx
	jmp    heapify_loop

heapify_finish:
	popq   %r13
	popq   %r12
	popq   %rbx
	ret
	
	
# Heapify_array
#     %rdi: last
#
heapify_array:
	pushq  %r12             # Save %r12 and use it to store 'last'
	rrmovq %rdi, %r12
	pushq  %rbx             # Save %rbx before using it for i

	irmovq $1, %rsi		# %rdi = last - 1
	subq   %rsi, %rdi
	irmovq $2, %rsi		# %rdi = (last - 1)/2
	divq   %rsi, %rdi
	rrmovq %rdi, %rbx       # i = %rdi
	
ha_loop:
	andq   %rbx, %rbx       # check if i < 0
	jl     ha_finish

	rrmovq %rbx, %rdi       # Set %rdi = i, %rsi = last      
	rrmovq %r12, %rsi
	pushq  %rdi
	call   heapify_node     # Heapify the node
        popq   %rdi
	
	irmovq $1, %rdi         # i--
	subq   %rdi, %rbx
	jmp    ha_loop

ha_finish:
	popq   %rbx
	popq   %r12
	ret
	
#
# Extract_max
#     %rdi: last
#
extract_max:
	pushq  %rbx		# Save %rax before using it for max

	xorq   %rsi, %rsi       # max = heap[0]
	mrmovq heap(%rsi), %rbx

	rrmovq %rdi, %rdx
	addq   %rdx, %rdx       # %rcx = heap[last]
	addq   %rdx, %rdx
	addq   %rdx, %rdx
	mrmovq heap(%rdx), %rcx 
	rmmovq %rcx, heap(%rsi) # heap[0] = %rcx

	rrmovq %rdi, %rsi	# %rsi = last - 1
	irmovq $1, %rcx
	subq   %rcx, %rsi
	xorq   %rdi, %rdi       # %rdi = 0
	call   heapify_node     # Heapify the root

	rrmovq %rbx, %rax       # Set return value to max
	popq   %rbx
	ret

#
# Heapsort
#    %rdi: last
#
heapsort:

###
### THIS PART TO BE COMPLETED BY THE STUDENT.
### Delete the halt instruction and 
### add the code that implements the heapsort
### function as described by the C function
### from the previous section.

    pushq %rbx				 # Save %rbx 
	rrmovq %rdi, %rbx	     # %rbx = last
	call   heapify_array     # heapify_array(last)
    rrmovq %rbx, %rbp        # %rbp = i & i = last

hsloop:
    xorq %r14, %r14     # %r14 = 0
	subq %r14, %rbp     # if i < 0 goto end
	jl end

	rrmovq %rbp, %rdi   # %rdi = i
	rrmovq %rbp, %r12   # %r12 = i
	call   extract_max  # extract_max(i)
    addq %r12, %r12	    # %r12 = 8*i
    addq %r12, %r12
    addq %r12, %r12    
    rmmovq %rax, heap(%r12) # heap[i] = extract_max(i)
    irmovq $1, %r13	    # %r13 = 1
	subq %r13, %rbp     # i--
	jmp hsloop		    # goto hsloop

end:
	popq %rbx		
	ret
	
#
# Array to sort
#
.pos 0x1000
heap:	.quad 1
        .quad 2
        .quad 6
        .quad 2
        .quad 4
	.quad 17
	.quad 11
	.quad 16
	.quad 8
	.quad 13
	.quad 13
	.quad 9
	.quad 21

size:   .quad 13
	
#
# Stack (32 64 bit words is more than enough here).
#
.pos 0x3000
top:	            .quad 0x00000000,0x20     # top of stack.
bottom:             .quad 0x00000000          # bottom of stack.
