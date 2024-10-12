#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "front.h"
#define MAX_LINE_LEN 81
/*Checking that the memory allocation was done correctly*/
#define CHECK_ALLOC(ptr) \
    if (ptr == NULL) { \
        fprintf(stderr, "Memory allocation failed"); \
        exit(1); \
    }
 /*Data structure for macro*/
typedef struct Macro{
	char * macro_name;
	char *lines;
}Macro;
/*Declaration of the functions*/
char * findWord(char **line);
void checkSpace(char **line);
int find_macro_name(int length, Macro * arr_macro, char * one_word, FILE * am_file);
int extraText(char **line);
int isInstruction(char * one_word);
int isLable(char * word);
int isRegister(char * word);
int chackOperandMethod(char * word);
void checkOperand(char **line, int commandNum, struct dataLine *dataLine);
int isComma(char **line);
void checkOperands(char **line, int commandNum, struct dataLine *dataLine);
void updateOperand(char * currWord, int methodType, struct dataLine *dataLine, int i);
void checkString(char **line, struct dataLine *dataLine);
void checkEntryAndExtern(char **line, struct dataLine *dataLine);
void checkData(char **line, struct dataLine *dataLine);
struct symbol * searchSymbol(struct symbol * symbolTable, int symbolCount, char * symbolName);
struct external * searchExternal(struct external * externalTable, int externalCount, char * externalName);

#endif
