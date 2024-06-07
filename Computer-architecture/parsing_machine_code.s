# Ankit Sharma
# 2020CSB1072
################################ How to check? ######################################################

# starting from location 0x10001000, values are stored as word, in order given
# in comments, as address increases.

#####################################################################################################


#These are for my refernce to easily calculate needed bitmasks
#R 0000000 00000 00000 000 00000 0000000
# I 000000000000 00000 000 00000 0000000
#S 0000000 00000 00000 000 00000 0000000
#SB 0000000 00000 00000 000 00000 0000000


.data
################################ Input ###############################################################

#machine code we want to identify
cmd : .word 0x00940263

################################ Don't change anything below this line ################################

#address where we want to start storing
sl: .word 0x10001000
#bitmask to extract opcode 
opcd_msk: .word 0x0000007F
#bitmask to extrat rd (if present)
rd_msk: .word 0x00000F80

#all operation codes
op_R: .word 51
op_I: .word 3
op_S: .word 35
op_SB: .word 99
op_slti: .word 19

#all bit-masks to extract
rs2_msk: .word 0x01F00000 #srli 20
rs1_msk: .word 0x000F8000 #srli 15
#bitmask for imm in R type
imm_msk: .word 0xFFF00000 #srli 20
#S type
S_imm_msk1: .word 0x00000F80 #srli 7
S_imm_msk2: .word 0xFE000000 #srli 20
#SB type
SB_imm_msk1: .word 128 #slli 3				imm[11]
SB_imm_msk2: .word 3840 #srli 8				imm[1:4]
SB_imm_msk3: .word 0x80000000 #srli 20		imm[12]
SB_imm_msk4: .word 0x7E000000 #srli 21		imm[5:10]

func3_msk: .word 0x00007000 # srli 12




.text
beq x0 x0 main	

main:
	#storing all saved registers into stack
	addi x2 x2 -40
    sw x23 32(x2)
    sw x24 24(x2)
    sw x25 16(x2)
    sw x27 8(x2)
    sw x26 0(x2)
    
    #loading machine code
    lw x26 cmd
    
    #loading storing address
    lw x25 sl
    
    #op_code
    lw x27 opcd_msk		#loading bitmask
    and x31 x27 x26		#getting opcode
    sw x31 0(x25)		#storing opcode
    addi x25 x25 4		#increasing address pointer by 4 
    
    #loading operator and checking/branching if matched
    lw x24 op_R
    beq x31 x24 R_oper
    lw x24 op_I
    beq x31 x24 I_oper
    lw x24 op_S
    beq x31 x24 S_oper
    lw x24 op_SB
    beq x31 x24 SB_oper
    lw x24 op_slti
    beq x31 x24 I_oper
    
    
    R_oper:
    	#rs1
    	lw x27 rs1_msk
        and x31 x27 x26
        srli x31 x31 15
        sw x31 0(x25)
        addi x25 x25 4
        #rs2
    	lw x27 rs2_msk
        and x31 x27 x26
        srli x31 x31 20
        sw x31 0(x25)
        addi x25 x25 4
        #rd
        lw x27 rd_msk
        and x31 x27 x26
        srli x31 x31 7
        sw x31 0(x25)
        addi x25 x25 4
        #Func3
        lw x27 func3_msk
        and x31 x27 x26
        srli x31 x31 12
        sw x31 0(x25)
        addi x25 x25 4
        #Funct7
        lw x27 S_imm_msk2
        and x31 x27 x26
        srli x31 x31 25
        sw x31 0(x25)
        addi x25 x25 4
		
    	beq x0 x0 exit
    
    I_oper:
    	#rs1
    	lw x27 rs1_msk
        and x31 x27 x26
        srli x31 x31 15
        sw x31 0(x25)
        addi x25 x25 4
        
		#rd
        lw x27 rd_msk
        and x31 x27 x26
        srli x31 x31 7
        sw x31 0(x25)
        addi x25 x25 4
        
        #Imm
        lw x27 imm_msk
        and x31 x27 x26
        srli x31 x31 20
        sw x31 0(x25)
        addi x25 x25 4
        
        #Func3
        lw x27 func3_msk
        and x31 x27 x26
        srli x31 x31 12
        sw x31 0(x25)
        addi x25 x25 4
        
    	beq x0 x0 exit
        
    S_oper:
    	#rs1
    	lw x27 rs1_msk
        and x31 x27 x26
        srli x31 x31 15
        sw x31 0(x25)
        addi x25 x25 4
        
        #rs2
    	lw x27 rs2_msk
        and x31 x27 x26
        srli x31 x31 20
        sw x31 0(x25)
        addi x25 x25 4
        
        #imm
        lw x27 S_imm_msk1
        and x31 x27 x26
        srli x31 x31 7
        lw x27 S_imm_msk2
        and x23 x27 x26
        srli x23 x23 20
        add x31 x31 x23
        sw x31 0(x25)
        addi x25 x25 8
        
        #Func3
        lw x27 func3_msk
        and x31 x27 x26
        srli x31 x31 12
        sw x31 0(x25)
        addi x25 x25 4
        
        beq x0 x0 exit
        
    SB_oper:
    	#rs1
    	lw x27 rs1_msk
        and x31 x27 x26
        srli x31 x31 15
        sw x31 0(x25)
        addi x25 x25 4
        
        #rs2
    	lw x27 rs2_msk
        and x31 x27 x26
        srli x31 x31 20
        sw x31 0(x25)
        addi x25 x25 4
        
        #imm
        lw x27 SB_imm_msk3
        and x31 x27 x26
        srli x31 x31 20
        lw x27 SB_imm_msk1
        and x23 x27 x26
        slli x23 x23 3
        add x31 x31 x23
        lw x27 SB_imm_msk4
        and x23 x27 x26
        srli x23 x23 21
        add x31 x31 x23
        lw x27 SB_imm_msk2
        and x23 x27 x26
        srli x23 x23 8
        add x31 x31 x23
        slli x31 x31 1
        sw x31 0(x25)
        addi x25 x25 4
        beq x0 x0 exit
        
        #Func3
        lw x27 func3_msk
        and x31 x27 x26
        srli x31 x31 12
        sw x31 0(x25)
        addi x25 x25 4
    	
    exit:   	
        lw x23 32(x2)
        lw x24 24(x2)
        lw x25 16(x2)
        lw x27 8(x2)
        lw x26 0(x2)
        addi x2 x2 40
    
    
    
	
	
    






