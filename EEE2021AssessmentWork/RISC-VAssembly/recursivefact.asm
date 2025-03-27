.data
test_input: .word 3 6 7 8

.text
main:
	add t0, x0, x0
	addi t1, x0, 4
	la t2, test_input
main_loop:
	beq t0, t1, main_exit
	slli t3, t0, 2
	add t4, t2, t3
	lw a0, 0(t4)

	addi sp, sp, -20
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw t3, 12(sp)
	sw t4, 16(sp)

	jal ra, factorial

	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw t3, 12(sp)
	lw t4, 16(sp)
	addi sp, sp, 20

	addi a1, a0, 0
	addi a0, x0, 1
	ecall # Print Result
	addi a1, x0, ' '
	addi a0, x0, 11
	ecall
	
	addi t0, t0, 1
	jal x0, main_loop
main_exit:  
	addi a0, x0, 10
	ecall # Exit

factorial:
    # Base case: if n <= 1, return 1
    addi t0, x0, 1       # t0 = 1 (for comparison and return value for base case)
    bgt a0, t0, recursive # if a0 > 1, go to recursive case
    addi a0, t0, 0       # otherwise, set a0 = 1 (factorial of 0 or 1 is 1)
    jalr x0, 0(ra)       # return to caller
    
recursive:
    # Save the current state before the recursive call
    addi sp, sp, -8      # Allocate space on the stack
    sw ra, 4(sp)         # Save return address
    sw a0, 0(sp)         # Save current n
    
    # Calculate factorial(n-1)
    addi a0, a0, -1      # a0 = n-1
    jal ra, factorial    # Call factorial recursively with n-1
    
    # After recursive call returns, multiply n * factorial(n-1)
    lw t0, 0(sp)         # Load original n from stack
    mul a0, a0, t0       # a0 = n * factorial(n-1)
    
    # Restore return address and clean up stack
    lw ra, 4(sp)         # Restore return address
    addi sp, sp, 8       # Deallocate stack space
    
    # Return with result in a0