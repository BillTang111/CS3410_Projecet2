All the instructions in Table A (see below) are implemented: 
Table A
Immediate arithmetic	ADDIU, ANDI, ORI, XORI, SLTI, SLTIU
Register arithmetic	ADDU, SUBU, AND, OR, XOR, NOR, SLT, SLTU
Move	MOVN, MOVZ
Shifts (constant and variable)	SLL, SRL, SRA, SLLV, SRLV, SRAV
Immediate load	LUI

All the instructions in Table B (see below) have not been implemented. However, we have been working on the control signal of table B.

Table B
Jumps (with one delay slot)	J, JR, JAL, JALR
Branches (with one delay slot)	BEQ, BNE, BLEZ, BGTZ, BLTZ, BGEZ
Memory Load/Store (little endian, with pipeline stall if needed) LW, LB, LBU, SW, SB

Fixed Problem: 
Fixed the forwarding problem when immediate are used and $Rs,$Rd = $Rd in last program: 

For example: 
ADDIU $10,$0,-2047
ADDIU $10, $10, 10010
