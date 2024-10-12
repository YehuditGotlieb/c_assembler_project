#include "front.h"
#include "twoPasses.h"
#include "functions.h"
/*A function that handles the first pass*/
int firstPass(struct translationUnit * program, char * amFileName, FILE * amFile){
	char line[MAX_LINE_LEN];
	int flagError = 0;
	int ic = 100, dc = 0;
	int lineCounter = 0;
	struct dataLine dataLine = {0};
	struct symbol * foundSymbol;
	struct symbol * temp;
	int * temp2;
	struct symbol **temp3;
	char symbolName[MAX_LABLE_LENGTH];
	char * stringTemp ;
	char * currentChar;
	int i;
    while (fgets(line, MAX_LINE_LEN, amFile)) {/* Loop through each line in the assembly file */
        lineCounter++;
        dataLine = getDataLine(line);  /* Parse the current line */
        if (dataLine.error[0] != '\0') { /* Check if an error occurred*/
            flagError = 1;
            printf("Error in file: %s in line: %d type of error: %s\n", amFileName, lineCounter, dataLine.error);
            continue;
        }
        if (strlen(dataLine.lineOptions.directive.lable) > 0) /* Determine the symbol name based on the type of the line (directive or instruction) */
            strcpy(symbolName, dataLine.lineOptions.directive.lable);
        else 
            strcpy(symbolName, dataLine.lineOptions.instruction.lable);
        if ((((strlen(dataLine.lineOptions.directive.lable) > 0)) && 
            (dataLine.lineOptions.directive.directiveType == dataDir || dataLine.lineOptions.directive.directiveType == stringDir)) ||
            (strlen(dataLine.lineOptions.instruction.lable) > 0)) {/* Check if the symbol exists and handle accordingly */
            foundSymbol = searchSymbol(program->symbolTable, program->symbolCount, symbolName);
            if (foundSymbol) {
                /* Update the symbol type if it's an entry and set its address */
                if (foundSymbol->symbolType == symbolEntry) {
                    if (dataLine.typeOfLine == instructionLine) {
                        foundSymbol->symbolType = symbolEntryCode;
                        foundSymbol->address = ic;
                    } 
					else {
                        foundSymbol->symbolType = symbolEntryData;
                        foundSymbol->address = dc;
                    }
                } 
				else {
                    flagError = 1;
                    printf("Error in file: %s in line: %d type of error: redefining the label %s\n", amFileName, lineCounter, symbolName);
                }
            } 
			else {
                /* Add a new symbol to the symbol table if it doesn't exist */
                if (program->symbolCount == 0) {
                    program->symbolTable = (struct symbol *)malloc(1 * sizeof(struct symbol));
                    CHECK_ALLOC(program->symbolTable);
                } 
				else {
                    temp = (struct symbol *)realloc(program->symbolTable, (program->symbolCount + 1) * sizeof(struct symbol));
                    CHECK_ALLOC(temp);
                    program->symbolTable = temp;
                }
                strcpy(program->symbolTable[program->symbolCount].symbolName, symbolName);
                
                /* Set the symbol type and address based on the line type */
                if (dataLine.typeOfLine == instructionLine) {
                    program->symbolTable[program->symbolCount].symbolType = symbolCode;
                    program->symbolTable[program->symbolCount].address = ic;
                } else {
                    program->symbolTable[program->symbolCount].symbolType = symbolData;
                    program->symbolTable[program->symbolCount].address = dc;
                }
                program->symbolCount++;
            }
        }
        /* Update the instruction counter (IC) based on the type of instruction */
        if (dataLine.typeOfLine == instructionLine) {
            ic++;
            if (((dataLine.lineOptions.instruction.operands[0].operandType >= addressRg) && 
                (dataLine.lineOptions.instruction.operands[1].operandType >= addressRg)) || 
                ((dataLine.lineOptions.instruction.instructionType > 4) && dataLine.lineOptions.instruction.instructionType < 14)) {
                ic++;
            } else if (dataLine.lineOptions.instruction.instructionType < 5) {
                ic += 2;
            }
        }
        /* Handle directives and update the data counter (DC) */
        else if (dataLine.typeOfLine == directiveLine) {
            if (dataLine.lineOptions.directive.directiveType == dataDir) {
                if (program->imageData == NULL) {
                    program->imageData = (int *)malloc(dataLine.lineOptions.directive.countNumbers * sizeof(int));
                    CHECK_ALLOC(program->imageData);
                } else {
                    temp2 = (int *)realloc(program->imageData, (program->DC + dataLine.lineOptions.directive.countNumbers) * sizeof(int));
                    CHECK_ALLOC(temp2);
                    program->imageData = temp2;
                }
                memcpy(&program->imageData[program->DC], dataLine.lineOptions.directive.directiveValue.numbers, dataLine.lineOptions.directive.countNumbers * sizeof(int));
                dc += dataLine.lineOptions.directive.countNumbers;
                free(dataLine.lineOptions.directive.directiveValue.numbers);
                dataLine.lineOptions.directive.directiveValue.numbers = NULL;
                program->DC = dc;
            } else if (dataLine.lineOptions.directive.directiveType == stringDir) {
                if (program->imageData == NULL) {
                    program->imageData = (int *)malloc((strlen(dataLine.lineOptions.directive.directiveValue.string) + 1) * sizeof(int));
                    CHECK_ALLOC(program->imageData);
                } else {
                    temp2 = realloc(program->imageData, (program->DC + strlen(dataLine.lineOptions.directive.directiveValue.string) + 1) * sizeof(int));
                    CHECK_ALLOC(temp2);
                    program->imageData = temp2;
                }
                stringTemp = (char *)malloc(strlen(dataLine.lineOptions.directive.directiveValue.string) + 1);
                CHECK_ALLOC(stringTemp);
                strcpy(stringTemp, dataLine.lineOptions.directive.directiveValue.string);
                free(dataLine.lineOptions.directive.directiveValue.string);
                dataLine.lineOptions.directive.directiveValue.string = NULL;
                currentChar = stringTemp;
                while (*currentChar) {
                    program->imageData[dc++] = (int)(*currentChar);  /* Cast character to integer */
                    currentChar++;
                }
                program->imageData[dc++] = (int)('\0');  /* End string with null character */
                free(stringTemp);
                program->DC = dc;
            } else if (dataLine.lineOptions.directive.directiveType >= externDir) {
                foundSymbol = searchSymbol(program->symbolTable, program->symbolCount, dataLine.lineOptions.directive.directiveValue.lable);
                if (dataLine.lineOptions.directive.directiveType == entryDir && foundSymbol) {
                    if (foundSymbol->symbolType == symbolCode) 
                        foundSymbol->symbolType = symbolEntryCode;
                    else if (foundSymbol->symbolType == symbolData) 
                        foundSymbol->symbolType = symbolEntryData;
                    else {
                        flagError = 1;
                        printf("Error in file: %s in line: %d type of error: Repeated definition of entry %s\n", amFileName, lineCounter, symbolName);
                    }
                } else if (!foundSymbol) {
                    if (program->symbolCount == 0) {
                        program->symbolTable = (struct symbol *)malloc(1 * sizeof(struct symbol));
                        CHECK_ALLOC(program->symbolTable);
                    } else {
                        temp = (struct symbol *)realloc(program->symbolTable, (program->symbolCount + 1) * sizeof(struct symbol));
                        CHECK_ALLOC(temp);
                        program->symbolTable = temp;
                    }
                    strcpy(program->symbolTable[program->symbolCount].symbolName, dataLine.lineOptions.directive.directiveValue.lable);
                    if (dataLine.lineOptions.directive.directiveType == externDir) 
                        program->symbolTable[program->symbolCount].symbolType = symbolExtern;
                    else 
                        program->symbolTable[program->symbolCount].symbolType = symbolEntry;
                    program->symbolCount++;
                } else {
                    flagError = 1;
                    printf("Error in file: %s in line: %d type of error: redefinition of symbol %s\n", amFileName, lineCounter, symbolName);
                }
            }
        }
	}
	for(i = 0; i < program->symbolCount; i++){
		if(program->symbolTable[i].symbolType == symbolEntryData || program->symbolTable[i].symbolType == symbolData)
			program->symbolTable[i].address += ic;
		if(program->symbolTable[i].symbolType >= symbolEntryCode){
			if(program->entriesCount == 0){
				program->entries = (struct symbol **)malloc(1 * sizeof(struct symbol));
				CHECK_ALLOC(program->entries);
			}else{
				temp3 = (struct symbol **)realloc(program->entries, (program->entriesCount + 1) * sizeof(struct symbol));
				CHECK_ALLOC(temp3);
				program->entries = temp3;

			}
			program->entries[program->entriesCount] = &program->symbolTable[i];
			program->entriesCount++;
		}
		if(program->symbolTable[i].symbolType == symbolEntry){
			flagError = 1;
			printf("Error in file: %s type of error:  entry that was not defined anywhere:  %s\n", amFileName , symbolName);
		}

	}
	return flagError;
}

int secondPass(struct translationUnit * program, char * amFileName, FILE * amFile){
	char line[MAX_LINE_LEN];
	int lineCounter = 0;
	int flagError = 0;
	int i = 0;
	int * reallocTemp;
	struct external *temp;
	struct dataLine dataLine = {0};
	struct symbol * foundSymbol;
	struct external * fountExternal;
	while(fgets(line, MAX_LINE_LEN, amFile)){
		lineCounter++;
		dataLine = getDataLine(line);

		/** If the line is a directive */
		if (dataLine.typeOfLine == directiveLine){
			/** If the directive type is data */
			if (dataLine.lineOptions.directive.directiveType == dataDir){
				free(dataLine.lineOptions.directive.directiveValue.numbers);
				dataLine.lineOptions.directive.directiveValue.numbers = NULL;
			}
			/** If the directive type is string */
			else if (dataLine.lineOptions.directive.directiveType == stringDir){
				free(dataLine.lineOptions.directive.directiveValue.string);
				dataLine.lineOptions.directive.directiveValue.string = NULL;
			}
		}

		/** If the line is an instruction */
		if(dataLine.typeOfLine == instructionLine){
			/** Allocate memory for the image code array if IC is 0 */
			if(program->IC == 0){
				program->imageCode = (int *)malloc(sizeof(int));
				CHECK_ALLOC(program->imageCode);
			} 
			/** Reallocate memory for the image code array */
			else{
				reallocTemp = (int *)realloc(program->imageCode, (program->IC + 1) * sizeof(int));
				CHECK_ALLOC(reallocTemp);
				program->imageCode = reallocTemp;
			}

			/** Encode the instruction type */
			program->imageCode[program->IC] = dataLine.lineOptions.instruction.instructionType << 11;

			/** If the instruction has one operand */
			if(dataLine.lineOptions.instruction.instructionType < 14 && dataLine.lineOptions.instruction.instructionType > 4)
				program->imageCode[program->IC] |= 1 << (dataLine.lineOptions.instruction.operands[0].operandType + 3);

			/** If the instruction has two operands */
			if(dataLine.lineOptions.instruction.instructionType < 5){
				program->imageCode[program->IC] |= 1 << (dataLine.lineOptions.instruction.operands[0].operandType + 7);
				program->imageCode[program->IC] |= 1 << (dataLine.lineOptions.instruction.operands[1].operandType + 3);
			}

			/** Set the ARE bits */
			program->imageCode[program->IC] |= 1 << 2;
			program->IC++;

			/** If both operands are registers or address to a register */
			if(dataLine.lineOptions.instruction.operands[0].operandType >= addressRg &&
			   dataLine.lineOptions.instruction.operands[1].operandType >= addressRg){
				reallocTemp = (int *)realloc(program->imageCode, (program->IC + 1) * sizeof(int));
				CHECK_ALLOC(reallocTemp);
				program->imageCode = reallocTemp;
				program->imageCode[program->IC] = dataLine.lineOptions.instruction.operands[0].operandValue.reg << 6;
				program->imageCode[program->IC] |= dataLine.lineOptions.instruction.operands[1].operandValue.reg << 3;
				program->imageCode[program->IC] |= 1 << 2;
				program->IC++;
			}else{
				/** Loop over the operands */
				for(i = 0; i < 2; i++){
					/** In the case of a command without operands */
					if(dataLine.lineOptions.instruction.instructionType >= rts)
						break;
					/** In the case of a command with 1 operand */
					if(dataLine.lineOptions.instruction.instructionType >= clr && i == 1)
						break;
					
					reallocTemp = (int *)realloc(program->imageCode, (program->IC + 1) * sizeof(int));
					CHECK_ALLOC(reallocTemp);
					program->imageCode = reallocTemp;

					/** If the operand is a register or address to a register */
					if(dataLine.lineOptions.instruction.operands[i].operandType >= addressRg){
						if(dataLine.lineOptions.instruction.instructionType >= clr)
							program->imageCode[program->IC] = dataLine.lineOptions.instruction.operands[i].operandValue.reg << 3;
						else
							program->imageCode[program->IC] = dataLine.lineOptions.instruction.operands[i].operandValue.reg << (6 - (i * 3));
						program->imageCode[program->IC] |= 1 << 2;
					}
					/** If the operand is a label */
					else if(dataLine.lineOptions.instruction.operands[i].operandType == lable){
						foundSymbol = searchSymbol(program->symbolTable, program->symbolCount, dataLine.lineOptions.instruction.operands[i].operandValue.lable);
						if(foundSymbol){
							program->imageCode[program->IC] = foundSymbol->address << 3;
							/** If the symbol is external */
							if(foundSymbol->symbolType == symbolExtern){
								program->imageCode[program->IC] |= 1;
								fountExternal = searchExternal(program->externals, program->externalCount, foundSymbol->symbolName);
								if(fountExternal){
									reallocTemp = (int *)realloc(fountExternal->externalAddresses, (fountExternal->addressCount + 1) * sizeof(int));
									CHECK_ALLOC(reallocTemp);
									fountExternal->externalAddresses = reallocTemp;
									fountExternal->externalAddresses[fountExternal->addressCount] = program->IC + 100;
									fountExternal->addressCount++;
								}else{
									if(program->externalCount == 0){
										program->externals = (struct external *)malloc(sizeof(struct external));
										CHECK_ALLOC(program->externals);
									}else{
										temp = (struct external *)realloc(program->externals, (program->externalCount + 1) * sizeof(struct external));
										CHECK_ALLOC(temp);
										program->externals = temp;
									}
									program->externals[program->externalCount].externalAddresses = (int *)malloc(sizeof(int));
									strcpy(program->externals[program->externalCount].externalName, foundSymbol->symbolName);
									program->externals[program->externalCount].addressCount = 0;
									program->externals[program->externalCount].externalAddresses[0] = program->IC + 100;
									program->externals[program->externalCount].addressCount++;
									program->externalCount++;
								}
							}else
								program->imageCode[program->IC] |= 1 << 1;
						}else{
							flagError = 1;
							printf("Error in file: %s in line %d type of error: Undefined label: %s\n", amFileName, lineCounter, dataLine.lineOptions.instruction.operands[i].operandValue.lable);
						}
					}
					/** If the operand is an immediate value */
					else{
						program->imageCode[program->IC] = dataLine.lineOptions.instruction.operands[i].operandValue.immed << 3;
						program->imageCode[program->IC] |= 1 << 2;
					}
					program->IC++;
				}
			}
		}
	}
	return flagError;
}