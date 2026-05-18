//
// Created by Aser Elserwee on 16/05/2026.
//


#include <stdio.h>
#include <string.h>
#include "encoder.h"

// This function converts instruction name to opcode
int getOpcode(char name[10]) {
    if (strcmp(name, "ADD") == 0) return 0;
    if (strcmp(name, "SUB") == 0) return 1;
    if (strcmp(name, "MUL") == 0) return 2;
    if (strcmp(name, "MOVI") == 0) return 3;
    if (strcmp(name, "JEQ") == 0) return 4;
    if (strcmp(name, "AND") == 0) return 5;
    if (strcmp(name, "XORI") == 0) return 6;
    if (strcmp(name, "JMP") == 0) return 7;
    if (strcmp(name, "LSL") == 0) return 8;
    if (strcmp(name, "LSR") == 0) return 9;
    if (strcmp(name, "MOVR") == 0) return 10;
    if (strcmp(name, "MOVM") == 0) return 11;

    return -1;
}

// R-format:
// OPCODE | R1 | R2 | R3 | SHAMT
// 4 bits | 5  | 5  | 5  | 13
unsigned int encodeR(int opcode, int r1, int r2, int r3, int shamt) {

    unsigned int opcodePart = opcode << 28;
    unsigned int r1Part = r1 << 23;
    unsigned int r2Part = r2 << 18;
    unsigned int r3Part = r3 << 13;
    unsigned int shamtPart = shamt;

    unsigned int result = opcodePart | r1Part | r2Part | r3Part | shamtPart;

    return result;
}

// I-format:
// OPCODE | R1 | R2 | IMMEDIATE
// 4 bits | 5  | 5  | 18
unsigned int encodeI(int opcode, int r1, int r2, int immediate) {

    unsigned int opcodePart = opcode << 28;
    unsigned int r1Part = r1 << 23;
    unsigned int r2Part = r2 << 18;

    // immediate is 18 bits, so we keep only 18 bits
    unsigned int immediatePart = immediate & 0x3FFFF; // because its already in the last bits

    unsigned int result = opcodePart | r1Part | r2Part | immediatePart;

    return result;
}

// J-format:
// OPCODE | ADDRESS
// 4 bits | 28
unsigned int encodeJ(int opcode, int address) {

    unsigned int opcodePart = opcode << 28;

    // address is 28 bits, so we keep only 28 bits
    unsigned int addressPart = address & 0x0FFFFFFF;

    unsigned int result = opcodePart | addressPart;

    return result;
}

// This is the main encoder function.
// main.c will call this function.
unsigned int encodeInstruction(char line[100]) {

    char name[10];

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
    int shamt = 0;
    int immediate = 0;
    int address = 0;

    // Read the first word from the line
    // Example: "ADD R1 R2 R3"
    // name becomes "ADD"
    sscanf(line, "%s", name);

    int opcode = getOpcode(name);

    if (opcode == -1) {
        printf("Unknown instruction: %s\n", name);
        return 0;
    }

    // ADD R1 R2 R3
    // SUB R1 R2 R3
    // MUL R1 R2 R3
    // AND R1 R2 R3
    if (strcmp(name, "ADD") == 0 ||
        strcmp(name, "SUB") == 0 ||
        strcmp(name, "MUL") == 0 ||
        strcmp(name, "AND") == 0) {

        sscanf(line, "%s R%d R%d R%d", name, &r1, &r2, &r3);

        return encodeR(opcode, r1, r2, r3, 0);
    }

    // MOVI R1 IMM
    // R2 is 0 in the instruction format
    if (strcmp(name, "MOVI") == 0) {

        sscanf(line, "%s R%d %d", name, &r1, &immediate);

        return encodeI(opcode, r1, 0, immediate);
    }

    // JEQ R1 R2 IMM
    // XORI R1 R2 IMM
    // MOVR R1 R2 IMM
    // MOVM R1 R2 IMM
    if (strcmp(name, "JEQ") == 0 ||
        strcmp(name, "XORI") == 0 ||
        strcmp(name, "MOVR") == 0 ||
        strcmp(name, "MOVM") == 0) {

        sscanf(line, "%s R%d R%d %d", name, &r1, &r2, &immediate);

        return encodeI(opcode, r1, r2, immediate);
    }

    // JMP ADDRESS
    if (strcmp(name, "JMP") == 0) {

        sscanf(line, "%s %d", name, &address);

        return encodeJ(opcode, address);
    }

    // LSL R1 R2 SHAMT
    // LSR R1 R2 SHAMT
    // R3 is 0 in the instruction format
    if (strcmp(name, "LSL") == 0 ||
        strcmp(name, "LSR") == 0) {

        sscanf(line, "%s R%d R%d %d", name, &r1, &r2, &shamt);

        return encodeR(opcode, r1, r2, 0, shamt);
    }

    return 0;
}