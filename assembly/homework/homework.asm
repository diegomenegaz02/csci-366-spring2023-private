//==========================================================================================
// PROBLEM 1: Counting Up From 1 to 10 in Little Man Assembly
//==========================================================================================
START   LDA COUNT
        OUT
        ADD ONE
        STA COUNT
        LDA LOOPS
        SUB COUNT
        BRP START
        HLT
COUNT   DAT 1
ONE     DAT 1
LOOPS   DAT 10
// You can develop your code via the online emulator and then paste it in here
// Please use a jump in your solution
// Please use the OUT instruction to print values

//==========================================================================================
// PROBLEM 2: Counting Up From 1 to 10 in MIPS Assembly
//==========================================================================================
.data

.text
	addi $8, $0, 0
	addi $9, $0, 10
	startloop:
	bgt $8, $9, exit
	li $v0 , 1
	move $a0, $8
	syscall
	
	addi $8, $8 ,1
	j startloop
	
	exit:
	li $v0, 10
	syscall
	
	


// You can develop your code via the MARS emulator and then paste it in here
// Please use a jump in your solution
// Please use sys-calls to output the value
