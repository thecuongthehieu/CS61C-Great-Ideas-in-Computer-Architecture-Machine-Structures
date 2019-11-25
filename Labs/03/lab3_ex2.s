        .data
x:      .word 0
y:      .word 1

        .text
main: 	la	$t0, x
	lw	$t0, 0($t0)
	la	$t1, y
	lw	$t1, 0($t1)
	add	$t2, $t1, $t0
	add 	$t3, $t2, $t1
	add	$t4, $t3, $t2
	add 	$t5, $t4, $t3
	add 	$t6, $t5, $t4
	add 	$t7, $t6, $t5
	
	li 	$v0, 1
	addi	$a0, $t7, 0
	syscall
	li 	$v0, 10
	syscall
	
