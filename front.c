#include "front.h"
#include "functions.h"

/*The function that checks the integrity of an assembly command line*/
struct dataLine getDataLine(char * line){
	struct dataLine dataLine = {0};
	char currWord[30];
	char lableName[30];
	char* temp;
	int isLableAns = 0;
	int commandNum = 0;
	int lableFlag = 0;
	checkSpace(&line);
	if(line[0]=='\n'){/*In case the line is empty*/
		dataLine.typeOfLine = emptyLine;
		return dataLine;
	}
	if(line[0]==';'){/*In case the line is a comment line*/
		dataLine.typeOfLine = commentLine;
		return dataLine;
	}
	temp = findWord(&line);
	strcpy(currWord , temp);
	free(temp);
	if(*(currWord+strlen(currWord)-1) == ':'){/*Checking if the first word is a label, and if it is a label we will check if the label is correct*/
		currWord[strlen(currWord)-1] = '\0';
		isLableAns = isLable(currWord);
		if(isLableAns==1){/*in case the word is a lable*/
			if(isComma(&line)){/*Checking that there is no comma after the label*/
				strcpy(dataLine.error, "Unnecessary comma");
				return dataLine;
			}
			lableFlag = 1;
			strcpy(lableName, currWord);
			temp = findWord(&line);
			strcpy(currWord, temp);
			free(temp);
		}
		if(isLableAns==-1){/*in case the word is not a lable*/
			strcpy(dataLine.error, "The label name is incorrect");
			return dataLine;
		}
	}
	if(isComma(&line)){/*Checking that there is no comma*/
		strcpy(dataLine.error, "Unnecessary comma");
		return dataLine;
	}
	commandNum = isInstruction(currWord);
	if(commandNum>=0 && commandNum <= 15){/*In case the line is an instruction line*/
		dataLine.typeOfLine = instructionLine;
		if(lableFlag)
			strcpy(dataLine.lineOptions.instruction.lable , lableName);
		dataLine.lineOptions.instruction.instructionType = commandNum;
		if(commandNum == 14 || commandNum == 15){/*In the case of a command without operands*/
			if(extraText(&line))
				strcpy(dataLine.error, "Extra text at the end of the command");
			return dataLine;
		}
		else if(commandNum > 4 && commandNum < 14){/*In case the command receives one operand*/
			checkOperand(&line, commandNum, &dataLine);
			return dataLine;
		}
		else{/*In case the command receives two operand*/
			checkOperands(&line, commandNum, &dataLine);
			return dataLine;
		}
	}
	else if(commandNum>15){/*In case the line is an directive line*/
		dataLine.typeOfLine = directiveLine;
		if(lableFlag)/*Updating the label in the data structure*/
			strcpy(dataLine.lineOptions.directive.lable , lableName);
		dataLine.lineOptions.directive.directiveType = commandNum - 16;
		if(commandNum == 17){/*in case the command is string*/
			checkString(&line, &dataLine);
			return dataLine;
		}
		else if(commandNum == 18 || commandNum == 19){/*in case the command is entry or extern*/
            checkEntryAndExtern(&line, &dataLine);
            return dataLine;
        }
        else{/*in case the command is entry or data*/
        	checkData(&line, &dataLine);
        	return dataLine;
        }
	}
	else{/*In the case of an incorrect line*/
		strcpy(dataLine.error, "Invalid line");
		return dataLine;
	}
	return dataLine;
}


