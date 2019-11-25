# Sample MIPS program that writes to a new file.
#
 .data
fout: .asciiz "file2.txt" # filename for output
buffer: .asciiz "How are you?"
 .text
main:
 # Open (for writing) a file that does not exist
 la $a0, fout # output file name
 li $a1, 1 # flags
 li $v0, 13 # system call for open file
 syscall # open a file (file descriptor returned in $v0)
 move $t0, $v0 # save file descriptor in $t0

 # Write to file just opened
 li $v0, 15 # system call for write to file
 la $a1, buffer # address of buffer from which to write
 li $a2, 12 # hardcoded buffer length
 move $a0, $t0 # put the file descriptor in $a0
 syscall # write to file
 # Close the file
 li $v0, 16 # system call for close file
 move $a0, $t0 # Restore fd
 syscall # close file
 li $v0, 10 # system call for exit
 syscall 