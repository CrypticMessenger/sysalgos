# Ankit Sharma
# 2020CSB1072
################################ checking output #######################################################
# Go to location 0x10001000 to chck sorted array.
# .byte is used instead of .word to make the array elements continuous, helped in debugging too.
# algorithm used is O(n1 + n2), traverses through array, and compares element, insert least element of 
# the two and increses pointer for that array. if end of one array is reached, inserts remaining element
# of array into location
########################################################################################################

.data
################################ Input #################################################################
# arrays
arr1 : .byte 1 2 3 36 41 59
arr2: .byte 1 22 33 34 85 92 93 94
#len of arrays
n1: .word 6
n2: .word 8
################################ do not change anything below this line ################################
#store location
sl: .word 0x10001000

.text
beq x0 x0 main	

main:
	#load address of arrays
	la x11 arr1
    la x12 arr2
    #loading length of arrays
    lw x5 n1
    lw x6 n2
    #loading store address i.e. 0x10001000
    lw x7 sl
    #max address for array 1
    add x5 x5 x11
    #max address for array 2
    add x6 x6 x12
 	loop:
    	bge x11 x5 exit1
        bge x12 x6 exit2
        lb x28 0(x11)
        lb x29 0(x12)
        # if a[i]>=b[j] then insert b[j] in loaction
        bge x28 x29 insert1
        # else insert a[i]
        blt x28 x29 insert2
        insert1:
        	sb x29 0(x7)
             #increase address in memory by 1
            addi x7 x7 1
            addi x12 x12 1
            addi x30 x30 1
            beq x0 x0 loop
        insert2:
        	sb x28 0(x7)
            #increase address in memory by 1
            addi x7 x7 1
            addi x11 x11 1
            addi x30 x30 1
            beq x0 x0 loop
    exit1:
    	# if we reach end of one array then we insert other elements in order
    	bge x12 x6 exit
        lb x29 0(x12)
        sb x29 0(x7)
        addi x7 x7 1
        addi x12 x12 1
        beq x0 x0 exit1
    exit2:
    	# if we reach end of one array then we insert other elements in order
    	bge x11 x5 exit
        lb x29 0(x11)
        sb x29 0(x7)
        addi x7 x7 1
        addi x11 x11 1
        beq x0 x0 exit2 
    exit:
    
    
    
    
    
	
    
    
	
	
    






