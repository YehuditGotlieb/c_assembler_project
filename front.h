#ifndef FRONT_H
#define FRONT_H
#include <stdio.h>
#include <string.h>
#define MAX_LABLE_LENGTH 32
/*A data structure that contains the command type*/
struct dataLine{
	enum {
		instructionLine, directiveLine,	commentLine, emptyLine
	}typeOfLine;
	union{
		struct{
			char lable[MAX_LABLE_LENGTH];
			enum{
				mov, cmp, add, sub, lea,
				clr, not, inc, dec, jmp,
				bne, red, prn, jsr, rts,
				stop
			}instructionType;
			struct{
				enum{
					immed, lable, addressRg, registerOP
				}operandType;
				union{
					int immed;
					char lable[MAX_LABLE_LENGTH];
					int reg;
				}operandValue;
			}operands[2];
		}instruction;
		struct{
			char lable[MAX_LABLE_LENGTH];
			int countNumbers;
			enum{
				dataDir, stringDir, externDir, entryDir
			}directiveType;
			union{
				int * numbers;
				char * string;
				char lable[MAX_LABLE_LENGTH];
			}directiveValue;
		}directive;	
	}lineOptions;
	char error[100];
};
struct dataLine getDataLine(char * line);
#endif
