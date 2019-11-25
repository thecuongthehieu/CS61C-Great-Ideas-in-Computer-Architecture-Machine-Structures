.data
shouldben1:	.asciiz "Should be -1, and firstposshift and firstposmask returned: "
shouldbe0:	.asciiz "Should be 0 , and firstposshift and firstposmask returned: "
shouldbe16:	.asciiz "Should be 16, and firstposshift and firstposmask returned: "
shouldbe31:	.asciiz "Should be 31, and firstposshift and firstposmask returned: "

.text
main:
	la	$a0, shouldbe31
	jal	print_str
	lui	$a0, 0x8000	# should be 31
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x8000
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe16
	jal	print_str
	lui	$a0, 0x0001	# should be 16
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x0001
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe0
	jal	print_str
	li	$a0, 1		# should be 0
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	li	$a0, 1
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldben1
	jal	print_str
	move	$a0, $0		# should be -1
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	move	$a0, $0
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	li	$v0, 10
	syscall

first1posshift:
	### YOUR CODE HERE ###
	addi	$sp, $sp, -4
	sw	$ra, 0($sp)
	
	beq	$a0, $0, return1
	blt	$a0, $0 return0
	
	sll	$a0, $a0, 1
	jal	first1posshift
	addiu	$v0, $v0, -1
	j	return

return0:
	addi 	$v0, $0, 31
	j 	return
	
return1:
	addi	$v0, $0, -1
return:	
	lw	$ra, 0($sp)
	addi	$sp, $sp, 4
	jr	$ra

	
first1posmask:
	### YOUR CODE HERE ###
	lui	$s1, 0x8000

LOOP:	
	addi	$sp, $sp, -4
	sw	$ra, 0($sp)
	
	beqz	$a0, Return1
	
	and	$t0, $a0, $s1
	bnez	$t0, Return0
	
	srl	$s1, $s1, 1
	jal	LOOP
	addiu	$v0, $v0, -1
	j	Return

Return0:
	addi 	$v0, $0, 31
	j 	Return
	
Return1:
	addi	$v0, $0, -1
Return:	
	lw	$ra, 0($sp)
	addi	$sp, $sp, 4
	jr	$ra
				

print_int:
	move	$a0, $v0
	li	$v0, 1
	syscall
	jr	$ra

print_str:
	li	$v0, 4
	syscall
	jr	$ra

print_space:
	li	$a0, ' '
	li	$v0, 11
	syscall
	jr	$ra

print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra
