//
// Created by carson on 11/15/21.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assembler.h"

char *ASM_ERROR_UNKNOWN_INSTRUCTION = "Unknown Assembly Instruction";
char *ASM_ERROR_ARG_REQUIRED = "Argument Required";
char *ASM_ERROR_BAD_LABEL = "Bad Label";
char *ASM_ERROR_OUT_OF_RANGE = "Number is out of range";

//=========================================================
//  All the instructions available on the LMSM architecture
//=========================================================
const char *INSTRUCTIONS[28] =
        {"ADD", "SUB", "LDA", "STA", "BRA", "BRZ", "BRP", "INP", "OUT", "HLT", "COB", "DAT",
         "LDI",
         "JAL", "CALL", "RET",
         "SPUSH", "SPUSHI", "SPOP", "SDUP", "SDROP", "SSWAP", "SADD", "SSUB", "SMAX", "SMIN", "SMUL", "SDIV"
        };
const int INSTRUCTION_COUNT = 28;

//===================================================================
//  All the instructions that require an arg on the LMSM architecture
//===================================================================
const char *ARG_INSTRUCTIONS[11] =
        {"ADD", "SUB", "LDA", "STA", "BRA", "BRZ", "BRP", "DAT",
         "LDI",
         "CALL",
         "SPUSHI"
        };
const int ARG_INSTRUCTION_COUNT = 11;

//======================================================
// Constructors/Destructors
//======================================================

asm_instruction * asm_make_instruction(char* type, char *label, char *label_reference, int value, asm_instruction * predecessor) {
    asm_instruction *new_instruction = calloc(1, sizeof(asm_instruction));
    new_instruction->instruction = type;
    new_instruction->label = label;
    new_instruction->label_reference = label_reference;
    new_instruction->value = value;
    new_instruction->next = NULL;
    if (predecessor != NULL) {
        predecessor->next = new_instruction;
        new_instruction->offset = predecessor->offset + predecessor->slots;
    } else {
        new_instruction->offset = 0;
    }
    //Non Sudo = 1
    //Call =3
    //Squishi =2 Use strcmp returns 0
    if(strcmp(type,"CALL") == 0){
        new_instruction->slots = 3;
    }else if(strcmp(type, "SPUSHI") == 0){
        new_instruction->slots = 2;
    }else{
        new_instruction->slots = 1;
    }



    // TODO: set the  number of slots for the instruction into the slots field
    return new_instruction;
}

void asm_delete_instruction(asm_instruction *instruction) {
    if (instruction == NULL) {
        return;
    }
    asm_delete_instruction(instruction->next);
    free(instruction);
}

asm_compilation_result * asm_make_compilation_result() {
    asm_compilation_result *result = calloc(1, sizeof(asm_compilation_result));
    return result;
}

void asm_delete_compilation_result(asm_compilation_result *result) {
    asm_delete_instruction(result->root);
    free(result);
}

//======================================================
// Helpers
//======================================================
int asm_is_instruction(char * token) {
    for (int i = 0; i < INSTRUCTION_COUNT; ++i) {
        if (strcmp(token, INSTRUCTIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int asm_instruction_requires_arg(char * token) {
    for (int i = 0; i < ARG_INSTRUCTION_COUNT; ++i) {
        if (strcmp(token, ARG_INSTRUCTIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int asm_is_num(char * token){
    if (*token == '-') { // allow a leading negative
        token++;
        if (*token == '\0') {
            return 0;
        }
    }
    while (*token != '\0') {
        if (*token < '0' || '9' < *token) {
            return 0;
        }
        token++;
    }
    return 1;
}

int asm_find_label(asm_instruction *root, char *label) {
    if(label == root->label_reference ){
        return 0;
    }else if(root->next != NULL ){
        asm_find_label(root->next, label);
    }else{
        return -1;
    }
    // TODO - scan the linked list for the given label, return -1 if not found

}


//======================================================
// Assembly Parsing/Scanning
//======================================================
//FINAL BOSS DUN DUH DUN DUH DUN DUH DUN
void  asm_parse_src(asm_compilation_result * result, char * original_src){

    // copy over so strtok can mutate
    char * src = calloc(strlen(original_src) + 1, sizeof(char));
    strcat(src, original_src);
    asm_instruction * last_instruction = NULL;
    asm_instruction * current_instruction = NULL;
    char *label_refrence = NULL;
    char *instruction = NULL;
    char * label = NULL;
    int arg_val = 0;

    // INP having leading space
    //PUSH
    //SDUP
    //SPOP
    //OOUT
    //Three forms
        //Label <instruction>
        //<instruction>
        //<label> <instruction> <arg>
        //<inst> <arg>
     char *token = strtok(src,"\n");
     while( token != NULL){

         int is_inst = asm_is_instruction(token);

         if(token == NULL || !asm_is_instruction(token)){
             result-> error = ASM_ERROR_UNKNOWN_INSTRUCTION;
             return;
         }else{
             instruction = token;
             if(asm_instruction_requires_arg(instruction)){
                 char *argument = strtok(instruction, " ");
                 argument = strtok(NULL, "\n");

                 if(asm_is_num(argument)){
                     arg_val = atoi(argument);
                     if( arg_val < -999 || arg_val > 999){
                         result -> error = ASM_ERROR_OUT_OF_RANGE;
                     }
                 }else if (!is_inst){
                      label_refrence= argument;

                 }

             }
         }
         current_instruction = asm_make_instruction(instruction,label,label_refrence,arg_val,last_instruction);
         if(result->root ==NULL){
             result->root = current_instruction;
         }

         last_instruction = current_instruction;

             token = strtok(NULL,"\n");


         //advance to the next token
     }
    //TODO - generate a linked list of instructions and store the first into
    //       the result->root
    //
    //       generate the following errors as appropriate:
    //
    //       ASM_ERROR_UNKNOWN_INSTRUCTION - when an unknown instruction is encountered
    //       ASM_ERROR_ARG_REQUIRED        - when an instruction does not have a proper argument passed to it
    //       ASM_ERROR_OUT_OF_RANGE        - when a number argument is out of range (-999 to 999)
    //
    //       store the error in result->error
}

//======================================================
// Machine Code Generation
//======================================================

void asm_gen_code_for_instruction(asm_compilation_result  * result, asm_instruction *instruction) {
    //TODO - generate the machine code for the given instruction
    //
    // note that some instructions will take up multiple slots
    //
    // note that if the instruction has a label reference rather than a raw number reference
    // you will need to look it up with `asm_find_label` and, if the label does not exist,
    // report the error as ASM_ERROR_BAD_LABEL

    //Have to resolve the label wherever the label is in memory.(Return offset after traversing linked list)
    int value_for_instruction = instruction->value;
    if (strcmp("ADD", instruction->instruction) == 0) {
        result->code[instruction->offset] = 100 + value_for_instruction;
    } else if(strcmp("SUB",instruction->instruction ) == 0) {
        result->code[instruction->offset] = 200 + value_for_instruction;
    }else if(strcmp("STA",instruction->instruction) == 0 ) {
        result->code[instruction->offset] = 300 + value_for_instruction;
    }else if(strcmp("LDI",instruction->instruction) == 0) {
        result->code[instruction->offset] = 400 + value_for_instruction;
    }else if(strcmp("LDA",instruction->instruction) == 0 ) {
        result->code[instruction->offset] = 500 + value_for_instruction;
        if(instruction->label != NULL){
            //check label and then get refrence
        }
    }else if(strcmp("BRA",instruction->instruction) == 0) {
        result->code[instruction->offset] = 600 + value_for_instruction;
    }else if(strcmp("BRZ",instruction->instruction) == 0) {
        result->code[instruction->offset] = 700 + value_for_instruction;
    }else if(strcmp("BRP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 800 + value_for_instruction;
    }else if(strcmp("INP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 901;
    }else if(strcmp("OUT",instruction->instruction) == 0) {
        result->code[instruction->offset] = 902;
    }else if(strcmp("HALT",instruction->instruction) == 0 ) {
        result->code[instruction->offset] = 000;
    }else if(strcmp("SPUSH",instruction->instruction) == 0) {
        result->code[instruction->offset] = 920;
    }else if(strcmp("SPOP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 921;
    }else if(strcmp("SDUP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 922;
    }else if(strcmp("SDROP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 923;
    }else if(strcmp("SSWAP",instruction->instruction) == 0) {
        result->code[instruction->offset] = 924;
    }else if(strcmp("SADD",instruction->instruction) == 0) {
        result->code[instruction->offset] = 930;
    }else if(strcmp("SSUB",instruction->instruction) == 0) {
        result->code[instruction->offset] = 931;
    }else if(strcmp("SMUL",instruction->instruction) == 0) {
        result->code[instruction->offset] = 932;
    }else if(strcmp("SDIV",instruction->instruction) == 0) {
        result->code[instruction->offset] = 933;
    }else if(strcmp("SMAX",instruction->instruction) == 0) {
        result->code[instruction->offset] = 934;
    }else if(strcmp("SMIN",instruction->instruction) == 0) {
        result->code[instruction->offset] = 935;
    }else if(strcmp("SPUSHI",instruction->instruction) == 0) {
        result->code[instruction->offset] = 400 + value_for_instruction;
        result->code[instruction->offset + 1] = 920;
    }else if(strcmp("RET",instruction->instruction) == 0) {
        result->code[instruction->offset] = 911;
    }else if(strcmp("CALL",instruction->instruction) == 0) {
        result->code[instruction->offset] = 400 + value_for_instruction;
        result->code[instruction->offset + 1] = 920;
        result->code[instruction->offset + 2] = 911;
    }else if(strcmp("DAT",instruction->instruction) == 0) {
        result->code[instruction->offset] = value_for_instruction;
    }else if(strcmp("HLT",instruction->instruction) == 0 || strcmp("COB",instruction->instruction) ==0) {
        return;
    }else{
        int labelF = asm_find_label(instruction->instruction,instruction->label);
        if(labelF == 0 ){
            //tbd
        }else{
            result->error = ASM_ERROR_BAD_LABEL;
            return;
        }
    }

}

void asm_gen_code(asm_compilation_result * result) {
    asm_instruction * current = result->root;
    while (current != NULL) {
        asm_gen_code_for_instruction(result, current);
        current = current->next;
    }
}

//======================================================
// Main API
//======================================================

asm_compilation_result * asm_assemble(char *src) {
    asm_compilation_result * result = asm_make_compilation_result();
    asm_parse_src(result, src);
    asm_gen_code(result);
    return result;
}

