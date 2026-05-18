#include <stdio.h>
#include <string.h>
#include "encoder.h"
#include "decoder.h"
#include "executor.h"


unsigned int memory[2048];
int registers[32];
int PC=0;
unsigned int IR;



int opcode;

int resultRegister;
int operand1Register;
int operand2Register;

int shamt;
int immediate;
int address;

int operand1Value;
int operand2Value;

int ALUResult;
int memoryData;
int storeValue;



void fetch() {
    IR = memory[PC];
    PC++;
}


void memoryStage() {
    if (opcode == 10) {
        memoryData = memory[ALUResult];
    } else if (opcode == 11) {
        memory[ALUResult] = storeValue;
    }
}

void writeBack() {

    if (resultRegister == 0) {
        return;
    }

    if (opcode == 0 || opcode == 1 || opcode == 2 ||
        opcode == 3 || opcode == 5 || opcode == 6 ||
        opcode == 8 || opcode == 9) {

        registers[resultRegister] = ALUResult;
        }

    else if (opcode == 10) { // MOVR
        registers[resultRegister] = memoryData;
    }
}



int main(void) {
   FILE *file = fopen("/Users/asere/CLionProjects/ca project/text file.txt", "r"); //i open the file in read mode
    if (file == NULL) {
        printf("Error opening text file.\n");
        return 1;
    }
    char line[100];
    int instructionAddress = 0;

    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\n")] = '\0';

        memory[instructionAddress] = encodeInstruction(line);

        instructionAddress++;
    }

    fclose(file);

    printf("Program loaded successfully.\n");
    printf("Total instructions loaded = %d\n", instructionAddress);

    fetch();
    decode();
    execute();
    memoryStage();
    writeBack();

    printf("opcode = %d\n", opcode);
    printf("resultRegister = %d\n", resultRegister);
    printf("operand1Register = %d\n", operand1Register);
    printf("operand2Register = %d\n", operand2Register);
    printf("immediate = %d\n", immediate);
    printf("address = %d\n", address);
    printf("operand1Value = %d\n", operand1Value);
    printf("operand2Value = %d\n", operand2Value);

    return 0;

}


