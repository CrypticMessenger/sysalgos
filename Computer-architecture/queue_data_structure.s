# Ankit Sharma
# 2020CSB1072
########################## checking output ##########################
# 0x10005000 stores size of queue when ever 'S' instruction is encountered
# queue starts from 0x10001000, and grows above, but in case of dequeue ,starting adress shifts above.
# stick to the format given in questions i.e. adding space between every instruction/parameter.
# enter sequence of operation in "seque", and size of string in "len".
#####################################################################

.data
########################## Input ##########################################################

seque : .asciiz "E -4 S"
len : .word 6

########################## No need to change anything below this ##########################
ma : .word 0x10001000
sl: .word 0x10005000


.text
beq x0 x0 main	

dequeue:
        bne x30 x0 decrease_size
        addi x30 x30 0
        beq x0 x0 return_to_callee
        #decreasing size by 1
        decrease_size:
    		addi x30 x30 -1
            #changes old head of queue to zero (makes it easier to observe queue)
            sw x0 0(x5)
            #changes head
            addi x5 x5 4
        return_to_callee:
            #return to callee
            jalr x0 x1 0
    
size:
	#adding to stack
	addi x2 x2 -16
    sw x10 8(x2)
    sw x1 0(x2)
    #storing the size in the specified memory location
    sw x30 0(x9)
    #restoring old values
    lw x1 0(x2)
    lw x10 8(x2)
    addi x2 x2 16
    #returning to callee
    jalr x0 x1 0
    
enqueue:
	#adding to stack
	addi x2 x2 -16
    sw x10 8(x2)
    sw x1 0(x2)
    #storing value in next empty location in queue
	sw x10 0(x8)
    addi x8 x8 4
    #restoring old values
    lw x1 0(x2)
    lw x10 8(x2)
    addi x2 x2 16
    #increasing size by 1
    addi x30 x30 1
    #returning to callee
    jalr x0 x1 0
    
main:
	#stacking
	addi x2 x2 -32
    sw x18 24(x2)
    sw x27 16(x2)
    sw x23 8(x2)
    sw x24 0(x2)
	#start location
    lw x8 ma
    add x5 x8 x0
    #x9 store size location
    lw x9 sl
    # x30 stores total size
    add x30 x0 x0
    #load values of len and seque
    lw x28 len
    la x3 seque
    #store values of all characters
    addi x29 x0 69	#E
    addi x31 x0 68 #D
    addi x6 x0 83 #S 
    addi x27 x0 32 # ' '
    addi x24 x0 45 # -
    addi x23 x0 0 # boolean to number is negative.
    #i=0
    addi x7 x0 0
    begin:
    bge x7 x28 exit1
    #get char
    lb x18 0(x3)
    #branch on the basis of char
    beq x18 x27 jump
    beq x18 x29 jumpE
    beq x18 x6 jumpS
    beq x18 x31 jumpD
    jumpE:
    addi x3 x3 2
	beq x0 x0 Enq
    jumpS:
    beq x0 x0 Siz
    jumpD:
    beq x0 x0 Deq  
    jump:
    addi x7 x7 1
    addi x3 x3 1
    beq x0 x0 begin
    
    
    Enq:
    	# get next char after space
    	lb x22 0(x3)
        #check if it's negative symbol
       	bne x22 x24 continue
        #turn boolean x23 to 1 if yes
        addi x23 x0 1
        addi x3 x3 1
        addi x7 x7 1
        beq x0 x0 skip
        continue:
        # else keep the boolean as zero
        addi x23 x0 0
        skip:
        #load number
        lb x10 0(x3)
        #convert's ascii value to real value in decimal
        addi x10 x10 -48
        # x7 = x7 + 3 (x7 is keeping track if parsing is completed or not)
        addi x7 x7 3
#         x3 = x3 +1
		addi x3 x3 1
        start:
        lb x26 0(x3)
        beq x26 x0 exit_enq
        beq x26 x27 exit_enq
        add x25 x10 x0
        slli x10 x10 1
        slli x25 x25 3
        #x10 = 10 * x10 + x25 (deals with numbers, multidigit numbers)
        add x10 x10 x25
        addi x26 x26 -48
        add x10 x10 x26
#         x10 = x10 + x26
       	addi x3 x3 1
        addi x7 x7 1
        beq x0 x0 start
        exit_enq:
        #branching before pushing on basis of negative boolean
        bne x23 x0 inclu_neg 
        jal x1 enqueue
    	beq x0 x0 begin    
        inclu_neg:
        	sub x10 x0 x10
            jal x1 enqueue

        	
    Siz:
    	#size
     	jal x1 size
        addi x3 x3 1
    	addi x7 x7 1
        beq x0 x0 begin
    Deq:
    	#dequeue
        jal x1 dequeue
        addi x3 x3 1
    	addi x7 x7 1
        beq x0 x0 begin
    
    exit1:
    	# unstacking 
        lw x18 24(x2)
        lw x27 16(x2)
        lw x23 8(x2)
        lw x24 0(x2)
        addi x2 x2 32
	
    






