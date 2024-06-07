# Ankit Sharma
# 2020CSB1072
################################ checking output #######################################################
# Go to location 0x10001000 to check location of str1.
# Go to location 0x10002000 to check location of str2.
# check reg x7 to get number of different characters at a particular position
########################################################################################################


.data
############################### Input ##################################################################
str1: .asciiz "heyhowarEYo?"
str2: .asciiz "HelloIare9o_"
#starting addresses of strings
add1: .word 0x10000000
add2: .word 0x1000000D
#length of string
n: .word 12
############################### Do not change anything below this line #################################
main1: .word 0x10001000
main2: .word 0x10002000



.text
beq x0 x0 main	

shift_calc:
	addi x2 x2 -56
    sw x12 48(x2)
    sw x11 40(x2)
    sw x10 32(x2)
    sw x24 24(x2)
    sw x25 16(x2)
    sw x26 8(x2)
    sw x27 0(x2)
    #i=0
    add x27 x0 x0
    #shifting word 1 to destination address
    shift:
 	bge x27 x12 continue
        add x30 x27 x10
        lb x31 0(x30)
        sb x31 0(x7)
        addi x27 x27 1
    	addi x7 x7 1
        beq x0 x0 shift
    continue:
    
	lw x11 add2
    add x6 x0 x11
    addi x27 x0 0
    #shifting word 2 to destination address
    shift2:
 	bge x27 x12 continue2
        add x11 x27 x6
        lb x31 0(x11)
        sb x31 0(x8)
        addi x27 x27 1
    	addi x8 x8 1
        beq x0 x0 shift2
    continue2:
    
    lw x5 main1
    lw x6 main2
    add x26 x0 x5
    add x24 x0 x6
    # answer = 0
    addi x7 x0 0
    
    # i =0
    addi x29 x0 0
    
    loop:
        bge x29 x12 exit
        #address of a character in str1
        add x5 x26 x29
        #address of a character in str2
        add x6 x24 x29
        #loading characters
        lb x30 0(x5)
        lb x31 0(x6)

         
        #checking if chars are equal
        beq x30 x31 stay_same
        bge x30 x31 check
        
        slti x22 x31 123
        slti x21 x31 96
        xori x21 x21 1
        and x21 x22 x21
        
        slti x20 x30 91
        slti x19 x30 64
        xori x19 x19 1
        and x20 x20 x19
        
        and x20 x20 x21
        bne x20 x0 checking
        beq x0 x0 increase
        
        
         
        check:
        slti x22 x30 123
        slti x21 x30 96
        xori x21 x21 1
        and x21 x22 x21
        
        slti x20 x31 91
        slti x19 x31 64
        xori x19 x19 1
        and x20 x20 x19
        
        and x20 x20 x21
        bne x20 x0 checking
        beq x0 x0 increase
        

        
        checking:
        sub x30 x31 x30
        addi x27 x0 32
        beq x30 x27 stay_same
        addi x27 x0 -32
        beq x30 x27 stay_same    
        increase:
        # if any of above conditions not followed, then increase answer by 1
        addi x7 x7 1
        #loop
        addi x29 x29 1
        beq x0 x0 loop
    stay_same:
    	#loop
        addi x29 x29 1
        beq x0 x0 loop
        
     exit:
     	lw x12 48(x2)
     	lw x11 40(x2)
        lw x10 32(x2)
     	lw x24 24(x2)
        lw x25 16(x2)
        lw x26 8(x2)
        lw x27 0(x2)
        addi x2 x2 56
     	jalr x0 x1 0
        
main:
	 
    #register storing address of word 1(by default)
	lw x10 add1
    lw x11 add2
    #register storing length of string 
    lw x12 n
    
    #destination address for word 1
    lw x7 main1
    #destination address for word 2
    lw x8 main2
	jal x1 shift_calc

    
    
    
