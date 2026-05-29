#include <stdio.h>
#include <string.h>
#include "encoder.h"
#include "decoder.h"
#include "executor.h"


unsigned int memory[2048];
int registers[32];
int PC=0;
int clock = 1;

int instructionAddress = 0;
int branchTaken = 0;


// IF → ID
unsigned int IF_ID_IR;
int IF_ID_PC;
int IF_ID_valid = 0;
int IF_ID_instrNum;

// ID → EX
int ID_EX_opcode;
int ID_EX_resultRegister;
int ID_EX_operand1Register;
int ID_EX_operand2Register;
int ID_EX_shamt;
int ID_EX_immediate;
int ID_EX_address;
int ID_EX_operand1Value;
int ID_EX_operand2Value;
int ID_EX_PC;
int ID_EX_valid = 0;
int ID_EX_instrNum;


// EX → MEM
int EX_MEM_opcode;
int EX_MEM_resultRegister;
int EX_MEM_ALUResult;
int EX_MEM_storeValue;
int EX_MEM_valid = 0;
int EX_MEM_instrNum;

// MEM → WB
int MEM_WB_opcode;
int MEM_WB_resultRegister;
int MEM_WB_ALUResult;
int MEM_WB_memoryData;
int MEM_WB_valid = 0;
int MEM_WB_instrNum;


int EX_cycle = 0;
int ID_cycle = 0;
int nextFetchCycle = 1;

char regUpdate[100] = "";
char memUpdate[100] = "";


void fetch() {
    if (branchTaken == 1) {
        branchTaken = 0;
        return;
    }

    if (PC < instructionAddress) {
        IF_ID_IR = memory[PC];
        IF_ID_PC = PC;
        IF_ID_instrNum = PC + 1;
        IF_ID_valid = 1;
        // printf("IF Inputs: PC = %d\n", PC);
        PC++;

    } else {
        IF_ID_valid = 0;
    }
}


void memoryStage() {
    if (EX_MEM_valid == 0) {
        return;
    }

    MEM_WB_opcode = EX_MEM_opcode;
    MEM_WB_resultRegister = EX_MEM_resultRegister;
    MEM_WB_ALUResult = EX_MEM_ALUResult;
    MEM_WB_instrNum = EX_MEM_instrNum;

    if (EX_MEM_opcode == 10) { // MOVR
        MEM_WB_memoryData = memory[EX_MEM_ALUResult];
    }
    else if (EX_MEM_opcode == 11) { // MOVM
        memory[EX_MEM_ALUResult] = EX_MEM_storeValue;

        sprintf(memUpdate,
       "Memory Update: Memory[%d] = %d",
       EX_MEM_ALUResult,
       EX_MEM_storeValue);
    }

    MEM_WB_valid = 1;
    EX_MEM_valid = 0;
}

void writeBack() {

    if (MEM_WB_valid == 0) {
        return;
    }

    // not R0
    if (MEM_WB_resultRegister == 0) {
        MEM_WB_valid = 0; // clear since its last
        return;
    }


    if (MEM_WB_opcode == 0 ||  // ADD
        MEM_WB_opcode == 1 ||  // SUB
        MEM_WB_opcode == 2 ||  // MUL
        MEM_WB_opcode == 3 ||  // MOVI
        MEM_WB_opcode == 5 ||  // AND
        MEM_WB_opcode == 6 ||  // XORI
        MEM_WB_opcode == 8 ||  // LSL
        MEM_WB_opcode == 9) {  // LSR

        registers[MEM_WB_resultRegister] = MEM_WB_ALUResult;

        sprintf(regUpdate,
         "Register Update: R%d = %d",
         MEM_WB_resultRegister,
         MEM_WB_ALUResult);
        }
    // MOVR
    else if (MEM_WB_opcode == 10) {
        registers[MEM_WB_resultRegister] = MEM_WB_memoryData;

        sprintf(regUpdate,
        "Register Update: R%d = %d",
        MEM_WB_resultRegister,
        MEM_WB_memoryData);
    }


    // Clear the stage for the next cycle
    MEM_WB_valid = 0;
}

const char* opcodeName(int opcode) {
    switch(opcode) {
        case 0:  return "ADD";
        case 1:  return "SUB";
        case 2:  return "MUL";
        case 3:  return "MOVI";
        case 4:  return "JEQ";
        case 5:  return "AND";
        case 6:  return "XORI";
        case 7:  return "JMP";
        case 8:  return "LSL";
        case 9:  return "LSR";
        case 10: return "MOVR";
        case 11: return "MOVM";
        default: return "UNKNOWN";
    }
}

int main(void) {
    FILE *file = fopen("/Users/asere/CLionProjects/ca project/text file.txt", "r");

    if (file == NULL) {
        printf("Error opening text file.\n");
        return 1;
    }

    char line[100];

    for (int i = 0; i < 2048; i++) memory[i] = 0;
    for (int i = 0; i < 32; i++) registers[i] = 0;

    PC = 0;
    instructionAddress = 0;

    IF_ID_valid = 0;
    ID_EX_valid = 0;
    EX_MEM_valid = 0;
    MEM_WB_valid = 0;

    ID_cycle = 0;
    EX_cycle = 0;
    branchTaken = 0;
    nextFetchCycle = 1;
    clock = 1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) continue;

        memory[instructionAddress] = encodeInstruction(line);
        instructionAddress++;
    }

    fclose(file);

    while (memory[PC] != 0 ||
           IF_ID_valid == 1 ||
           ID_EX_valid == 1 ||
           EX_MEM_valid == 1 ||
           MEM_WB_valid == 1) {

        char IF_text[50] = "Empty";
        char ID_text[50] = "Empty";
        char EX_text[50] = "Empty";
        char MEM_text[50] = "Empty";
        char WB_text[50] = "Empty";

        char IF_inputs[100] = "";
        char ID_inputs[150] = "";
        char EX_inputs[250] = "";

        regUpdate[0] = '\0';
        memUpdate[0] = '\0';

        if (MEM_WB_valid == 1) {
            sprintf(WB_text, "Instruction %d", MEM_WB_instrNum);
        }

        writeBack();

        if (clock != nextFetchCycle || memory[PC] == 0) {
            if (EX_MEM_valid == 1) {
                sprintf(MEM_text, "Instruction %d", EX_MEM_instrNum);
            }

            memoryStage();
        }

        if (ID_EX_valid == 1) {

            int r1Value = registers[ID_EX_resultRegister];
            int r2Value = registers[ID_EX_operand1Register];
            int r3Value = registers[ID_EX_operand2Register];

            sprintf(EX_text, "Instruction %d", ID_EX_instrNum);

            if (ID_EX_opcode == 0 || ID_EX_opcode == 1 ||
                ID_EX_opcode == 2 || ID_EX_opcode == 5) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d, R2=R%d(%d), R3=R%d(%d)",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_operand2Register, r3Value);
            }
            else if (ID_EX_opcode == 3) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d, Immediate=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister,
                        ID_EX_immediate);
            }
            else if (ID_EX_opcode == 4) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d(%d), R2=R%d(%d), Immediate=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister, r1Value,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_immediate);
            }
            else if (ID_EX_opcode == 6) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d, R2=R%d(%d), Immediate=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_immediate);
            }
            else if (ID_EX_opcode == 7) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, Address=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_address);
            }
            else if (ID_EX_opcode == 8 || ID_EX_opcode == 9) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d, R2=R%d(%d), Shamt=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_shamt);
            }
            else if (ID_EX_opcode == 10) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, R1=R%d, Base=R%d(%d), Immediate=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_immediate);
            }
            else if (ID_EX_opcode == 11) {

                sprintf(EX_inputs,
                        "EX Inputs: Opcode=%s, StoreReg=R%d(%d), Base=R%d(%d), Immediate=%d",
                        opcodeName(ID_EX_opcode),
                        ID_EX_resultRegister, r1Value,
                        ID_EX_operand1Register, r2Value,
                        ID_EX_immediate);
            }
        }

        execute();

        if (IF_ID_valid == 1) {
            int opcode = IF_ID_IR >> 28;

            sprintf(ID_text, "Instruction %d", IF_ID_instrNum);

            sprintf(ID_inputs,
                    "ID Inputs: PC=%d, Opcode=%s",
                    IF_ID_PC,
                    opcodeName(opcode));
        }

        decode();

        if (clock == nextFetchCycle) {
            if (branchTaken == 0 && memory[PC] != 0) {
                sprintf(IF_text, "Instruction %d", PC + 1);

                sprintf(IF_inputs,
                        "IF Inputs: PC=%d",
                        PC);
            }

            fetch();
            nextFetchCycle = clock + 2;
        }

        printf("\nClock Cycle %d\n", clock);
        printf("IF  : %s\n", IF_text);
        printf("ID  : %s\n", ID_text);
        printf("EX  : %s\n", EX_text);
        printf("MEM : %s\n", MEM_text);
        printf("WB  : %s\n", WB_text);

        if (strlen(IF_inputs) > 0) {
            printf("%s\n", IF_inputs);
        }

        if (strlen(ID_inputs) > 0) {
            printf("%s\n", ID_inputs);
        }

        if (strlen(EX_inputs) > 0) {
            printf("%s\n", EX_inputs);
        }

        if (strlen(memUpdate) > 0) {
            printf("%s\n", memUpdate);
        }

        if (strlen(regUpdate) > 0) {
            printf("%s\n", regUpdate);
        }

        branchTaken = 0;
        clock++;
    }

    printf("\n=== FINAL REGISTER VALUES ===\n");
    for (int i = 0; i < 32; i++) {
        printf("R%d = %d\n", i, registers[i]);
    }

    printf("PC = %d\n", PC);

    printf("\n=== MEMORY CONTENTS ===\n");
    for (int i = 0; i < 2048; i++) {
        printf("Memory[%d] = %u\n", i, memory[i]);
    }

    return 0;
}