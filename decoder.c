//
// Created by Aser Elserwee on 17/05/2026.
//



#include <stdio.h>
#include "decoder.h"

extern unsigned int IR;
extern int registers[32];

extern int opcode;

extern int resultRegister;
extern int operand1Register;
extern int operand2Register;

extern int shamt;
extern int immediate;
extern int address;

extern int operand1Value;
extern int operand2Value;

int signExtend18(unsigned int value) {
    if (value & (1 << 17)) {
        return value | 0xFFFC0000;
    }

    return value;
}

void decodeR() {
    resultRegister = (IR >> 23) & 0x1F;
    operand1Register = (IR >> 18) & 0x1F;
    operand2Register = (IR >> 13) & 0x1F;
    shamt = IR & 0x1FFF;

    operand1Value = registers[operand1Register];
    operand2Value = registers[operand2Register];
}

void decodeI() {
    resultRegister = (IR >> 23) & 0x1F;
    operand1Register = (IR >> 18) & 0x1F;
    immediate = signExtend18(IR & 0x3FFFF);

    operand1Value = registers[operand1Register];
}

void decodeJ() {
    address = IR & 0x0FFFFFFF;
}

void decode() {
    opcode = IR >> 28;

    resultRegister = 0;
    operand1Register = 0;
    operand2Register = 0;

    shamt = 0;
    immediate = 0;
    address = 0;

    operand1Value = 0;
    operand2Value = 0;

    if (opcode == 0 ||
        opcode == 1 ||
        opcode == 2 ||
        opcode == 5 ||
        opcode == 8 ||
        opcode == 9) {

        decodeR();
        }
    else if (opcode == 3 ||
             opcode == 4 ||
             opcode == 6 ||
             opcode == 10 ||
             opcode == 11) {

        decodeI();
             }
    else if (opcode == 7) {
        decodeJ();
    }
    else {
        printf("Invalid opcode: %d\n", opcode);
    }
}