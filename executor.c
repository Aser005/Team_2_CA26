//
// Created by Aser Elserwee on 18/05/2026.

//


#include "executor.h"

extern int PC;

extern int opcode;

extern int resultRegister;
extern int operand1Register;
extern int operand2Register;

extern int shamt;
extern int immediate;
extern int address;

extern int operand1Value;
extern int operand2Value;

extern int registers[32];

extern int ALUResult;
extern int storeValue; 

int ALU(int input1, int input2, int operation) {

    if (operation == 0) return input1 + input2;                    // ADD
    if (operation == 1) return input1 - input2;                    // SUB
    if (operation == 2) return input1 * input2;                    // MUL
    if (operation == 3) return input1 & input2;                    // AND
    if (operation == 4) return input1 ^ input2;                    // XOR
    if (operation == 5) return input1 << input2;                   // LSL
    if (operation == 6) return ((unsigned int) input1) >> input2;  // LSR
    if (operation == 7) return input1 == input2;                   // EQUAL

    return 0;
}

void execute() {

    if (opcode == 0) {                 // ADD
        ALUResult = ALU(operand1Value, operand2Value, 0);
    }

    else if (opcode == 1) {            // SUB
        ALUResult = ALU(operand1Value, operand2Value, 1);
    }

    else if (opcode == 2) {            // MUL
        ALUResult = ALU(operand1Value, operand2Value, 2);
    }

    else if (opcode == 3) {            // MOVI
        ALUResult = immediate;
    }

    else if (opcode == 4) {            // JEQ
        if (ALU(registers[resultRegister], operand1Value, 7)) {
            PC = PC + immediate;
        }
    }

    else if (opcode == 5) {            // AND
        ALUResult = ALU(operand1Value, operand2Value, 3);
    }

    else if (opcode == 6) {            // XORI
        ALUResult = ALU(operand1Value, immediate, 4);
    }

    else if (opcode == 7) {            // JMP
        PC = address;
    }

    else if (opcode == 8) {            // LSL
        ALUResult = ALU(operand1Value, shamt, 5);
    }

    else if (opcode == 9) {            // LSR
        ALUResult = ALU(operand1Value, shamt, 6);
    }

    else if (opcode == 10) {           // MOVR
        ALUResult = ALU(operand1Value, immediate, 0);
    }

    else if (opcode == 11) {           // MOVM
        ALUResult = ALU(operand1Value, immediate, 0);
        storeValue = registers[resultRegister];
    }
}
