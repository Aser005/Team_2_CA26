//
// Created by Aser Elserwee on 17/05/2026.
//

#include "decoder.h"
#include "encoder.h"
#include <stdio.h>


extern unsigned int IF_ID_IR;
extern int IF_ID_PC;
extern int IF_ID_valid;


extern int ID_EX_opcode;
extern int ID_EX_resultRegister;
extern int ID_EX_operand1Register;
extern int ID_EX_operand2Register;
extern int ID_EX_shamt;
extern int ID_EX_immediate;
extern int ID_EX_address;
extern int ID_EX_operand1Value;
extern int ID_EX_operand2Value;
extern int ID_EX_PC;
extern int ID_EX_valid;

extern int ID_cycle;


extern int registers[32];

extern int IF_ID_instrNum;
extern int ID_EX_instrNum;

int signExtend18(unsigned int value) {
    if (value & (1 << 17)) {
        return value | 0xFFFC0000;
    }
    return value;
}


void decodeR() {
    ID_EX_resultRegister   = (IF_ID_IR >> 23) & 0x1F;
    ID_EX_operand1Register = (IF_ID_IR >> 18) & 0x1F;
    ID_EX_operand2Register = (IF_ID_IR >> 13) & 0x1F;
    ID_EX_shamt            = IF_ID_IR & 0x1FFF;


}


void decodeI() {
    ID_EX_resultRegister   = (IF_ID_IR >> 23) & 0x1F;
    ID_EX_operand1Register = (IF_ID_IR >> 18) & 0x1F;
    ID_EX_immediate        = signExtend18(IF_ID_IR & 0x3FFFF);

   
}


void decodeJ() {
    ID_EX_address = IF_ID_IR & 0x0FFFFFFF;
}


extern int ID_cycle;

void decode() {
    if (IF_ID_valid == 0) {
        ID_cycle = 0;
        return;
    }

    ID_cycle++;

    // First decode cycle — hold, don't pass downstream yet
    if (ID_cycle == 1) {
        return;
    }

    // Second decode cycle — now decode and pass to EX
    ID_cycle = 0;
    ID_EX_PC    = IF_ID_PC;
    ID_EX_instrNum = IF_ID_instrNum;
    ID_EX_valid = 1;
    IF_ID_valid = 0;






    int opcode = IF_ID_IR >> 28;
    ID_EX_opcode = opcode;

    if (opcode == 0 || opcode == 1 || opcode == 2 ||
        opcode == 5 || opcode == 8 || opcode == 9) {
        decodeR();
        }
    else if (opcode == 3 || opcode == 4 || opcode == 6 ||
             opcode == 10 || opcode == 11) {
        decodeI();
             }
    else if (opcode == 7) {
        decodeJ();
    }
    else {
        printf("Invalid opcode: %d\n", opcode);
        ID_EX_valid = 0;
    }
}