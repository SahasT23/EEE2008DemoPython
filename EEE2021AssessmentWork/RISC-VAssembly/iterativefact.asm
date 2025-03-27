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
    # Special case: if n <= 1, return 1
    addi t0, x0, 1       # t0 = 1 (for comparison and initial result value)
    bgt a0, t0, setup    # if a0 > 1, proceed to main calculation
    addi a0, t0, 0       # otherwise, set a0 = 1 (factorial of 0 or 1 is 1)
    jalr x0, 0(ra)       # return to caller
    
setup:
    # Initialise result to 1
    addi t1, t0, 0       # t1 = 1 (accumulated result)
    addi t2, a0, 0       # t2 = n (counter, starts at n)
    
loop:
    # Multiply current result by counter
    mul t1, t1, t2       # t1 = t1 * t2 (result = result * counter)
    
    # Decrement counter
    addi t2, t2, -1      # t2 = t2 - 1 (counter--)
    
    # Check if we've reached 1
    bgt t2, x0, loop     # if counter > 0, continue loop
    
    # Store final result in a0 and return
    addi a0, t1, 0       # a0 = t1 (return the final result)
    jalr x0, 0(ra)       # return to caller