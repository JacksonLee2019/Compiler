                    .text                                                # FinishSemantics
                    .globl      __start                                  # FinishSemantics
__start:                                                                 # FinishSemantics
                    jal         _main                                    # FinishSemantics
                    li          $v0         10                           # FinishSemantics
                    syscall                                              # FinishSemantics
_main:                                                                   # func entry
                    subu        $sp         $sp         4                # ProcFuncBody
                    sw          $ra         0($sp)                       # ProcFuncBody
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _x                             
                    li          $v0         4                              
                    la          $a0         L1                             
                    syscall                                                
                    lw          $t0         _x                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    li          $v0         5                            # read int syscall
                    syscall                                                
                    move        $t0         $v0                            
                    sw          $t0         _y                             
                    li          $v0         4                              
                    la          $a0         L3                             
                    syscall                                                
                    lw          $t0         _y                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L4                             
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    lw          $t0         _x                             
                    addi        $t1         $t0         1                  
                    sw          $t1         _x                             
                    li          $v0         4                              
                    la          $a0         L5                             
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    lw          $t0         _y                             
                    addi        $t1         $t0         -1                 
                    sw          $t1         _y                             
                    li          $v0         4                              
                    la          $a0         L1                             
                    syscall                                                
                    lw          $t0         _x                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L3                             
                    syscall                                                
                    lw          $t0         _y                             
                    li          $v0         1                              
                    move        $a0         $t0                            
                    syscall                                                
                    li          $v0         4                              
                    la          $a0         L2                             
                    syscall                                                
main_ret:                                                                  
                    lw          $ra         0($sp)                       # ProcFuncBody
                    addu        $sp         $sp         4                # ProcFuncBody
                    jr          $ra                                      # func return
                    .data                                                # FinishSemantics
_x:                 .word       0                                          
_y:                 .word       0                                          
L4:                 .asciiz     "incrementing x"                               
L1:                 .asciiz     "x = "                                     
L3:                 .asciiz     "y = "                                     
L2:                 .asciiz     "\n"                                       
L5:                 .asciiz     "decrementing y"                               
