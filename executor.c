

#include "executor.h"

extern int PC;
extern int registers[32];

// Pipeline registers
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

extern int EX_MEM_opcode;
extern int EX_MEM_resultRegister;
extern int EX_MEM_ALUResult;
extern int EX_MEM_storeValue;
extern int EX_MEM_valid;

extern int registers[32];
extern int PC;

extern int EX_cycle;
extern int ID_cycle;


extern int MEM_WB_instrNum;
extern int EX_MEM_instrNum;
extern int ID_EX_instrNum;
extern int IF_ID_instrNum;


extern int branchTaken;
extern int nextFetchCycle;
extern int clock;

// ALU function
int ALU(int input1, int input2, int operation) {
    if (operation == 0) return input1 + input2;                  // ADD
    if (operation == 1) return input1 - input2;                  // SUB
    if (operation == 2) return input1 * input2;                  // MUL
    if (operation == 3) return input1 & input2;                  // AND
    if (operation == 4) return input1 ^ input2;                  // XOR
    if (operation == 5) return input1 << input2;                 // LSL
    if (operation == 6) return ((unsigned int) input1) >> input2; // LSR
    if (operation == 7) return input1 == input2;                 // EQUAL
    return 0;
}



void execute() {
    if (ID_EX_valid == 0) {
        EX_cycle = 0;
        return;
    }

    EX_cycle++;

    // First execute cycle — compute nothing yet, just wait
    if (EX_cycle == 1) {
        return;
    }

    // Second execute cycle — finalize result and handle flush if needed
    EX_cycle = 0;
    EX_MEM_opcode = ID_EX_opcode;
    EX_MEM_resultRegister = ID_EX_resultRegister;
    EX_MEM_instrNum = ID_EX_instrNum;
    EX_MEM_valid = 1;

    ID_EX_operand1Value = registers[ID_EX_operand1Register];
    ID_EX_operand2Value = registers[ID_EX_operand2Register];

    ID_EX_valid = 0;



    if (ID_EX_opcode == 0) {               // ADD
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_operand2Value, 0);
    }
    else if (ID_EX_opcode == 1) {          // SUB
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_operand2Value, 1);
    }
    else if (ID_EX_opcode == 2) {          // MUL
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_operand2Value, 2);
    }
    else if (ID_EX_opcode == 3) {          // MOVI
        EX_MEM_ALUResult = ID_EX_immediate;
    }
    else if (ID_EX_opcode == 4) {          // JEQ
        if (ALU(registers[ID_EX_resultRegister], ID_EX_operand1Value, 7)) {
            PC = ID_EX_PC + 1 + ID_EX_immediate;

            IF_ID_valid = 0;

            ID_cycle = 0;
            branchTaken = 1;
            nextFetchCycle = clock + 1;
        }

    }
    else if (ID_EX_opcode == 5) {          // AND
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_operand2Value, 3);
    }
    else if (ID_EX_opcode == 6) {          // XORI
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_immediate, 4);
    }
    else if (ID_EX_opcode == 7) {          // JMP
        PC = (ID_EX_PC & 0xF0000000) | ID_EX_address;

        IF_ID_valid = 0;
        ID_cycle = 0;
        branchTaken = 1;
        nextFetchCycle = clock + 1;
    }
    else if (ID_EX_opcode == 8) {          // LSL
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_shamt, 5);
    }
    else if (ID_EX_opcode == 9) {          // LSR
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_shamt, 6);
    }
    else if (ID_EX_opcode == 10) {         // MOVR
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_immediate, 0);
    }
    else if (ID_EX_opcode == 11) {         // MOVM
        EX_MEM_ALUResult = ALU(ID_EX_operand1Value, ID_EX_immediate, 0);
        EX_MEM_storeValue = registers[ID_EX_resultRegister];
    }
}