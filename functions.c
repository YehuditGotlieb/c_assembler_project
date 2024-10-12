#include "functions.h"
#include "twoPasses.h"

/*A function that returns the first word currently on a command line*/
char *findWord(char **line){   
	char *theWord = (char *)malloc(sizeof(char));
	char *tempWord;
	if(theWord == NULL){
		printf("No place");
		exit(1);
	}
	checkSpace(line);
	*(theWord) = '\0';
	while(!isspace(**line)&& (**line) != '\0' && (**line)!= '\n' && (**line) != ','){
		tempWord = (char *)realloc(theWord, strlen(theWord) + sizeof(char)*2);
		if(tempWord == NULL){
			free(theWord);
			printf("No place");
			exit(1);
		}
		theWord = tempWord;
		*(theWord + strlen(theWord)+1) = '\0';
		*(theWord + strlen(theWord)) = (**line);
		(*line)++;
	} 
	return theWord;
}

/*A function that by passes the spaces in an input line*/
void checkSpace(char **line){
		while((**line) != '\0' && isspace(**line) && (**line)!='\n'){
			(*line)++;
		}	
}
/*A function that findes the macro name*/
int find_macro_name(int length, Macro *arr_macro, char * one_word, FILE *am_file){
	int i =0;
	while(i < length){
		if(!strcmp(one_word, arr_macro[i].macro_name) ){/*Checking whether the macro exists in a data structure*/
			if (arr_macro[i].lines != NULL)
			{
				fprintf(am_file, "%s", arr_macro[i].lines);
			}
			return 1;
		}
		i++;
	}
	return 0;
}	

/*Checking that there is no unnecessary text*/
int extraText(char **line){
	while((**(line) != '\0') && (**(line) != '\n')){
		if(!isspace(**line)){
			return 1;
		}
		(*line)++;
	}
	return 0; 
}

/*look for ',' and jump over it, return 1 if found, 0 if not found */
int isComma(char **line){
	checkSpace(line);
	if((**line) == ','){
		(*line)++;
		return 1;
	}
	return 0;
}

/*Checking if the word is a directive or an instruction*/
int isInstruction(char * one_word){
	if (strcmp(one_word, "mov") == 0)return 0;
	else if (strcmp(one_word, "cmp") == 0)return 1;
	else if (strcmp(one_word, "add") == 0)return 2;
	else if (strcmp(one_word, "sub") == 0)return 3;
	else if (strcmp(one_word, "lea") == 0)return 4;
	else if (strcmp(one_word, "clr") == 0)return 5;
	else if (strcmp(one_word, "not") == 0)return 6;
	else if (strcmp(one_word, "inc") == 0)return 7;
	else if (strcmp(one_word, "dec") == 0)return 8;
	else if (strcmp(one_word, "jmp") == 0)return 9;
	else if (strcmp(one_word, "bne") == 0)return 10;
	else if (strcmp(one_word, "red") == 0)return 11;
	else if (strcmp(one_word, "prn") == 0)return 12;
	else if (strcmp(one_word, "jsr") == 0)return 13;
	else if (strcmp(one_word, "rts") == 0)return 14;
	else if (strcmp(one_word, "stop") == 0)return 15;
	else if (strcmp(one_word, ".data") == 0)return 16;
	else if (strcmp(one_word, ".string") == 0)return 17;
	else if (strcmp(one_word, ".extern") == 0)return 18;
	else if (strcmp(one_word, ".entry") == 0)return 19;
	else return -1;
}

/*Checks if the word there belongs to a register, if so, returns the register number*/
int isRegister(char * word){
	if (strcmp(word, "r0") == 0)return 0;
	else if (strcmp(word, "r1") == 0)return 1;
	else if (strcmp(word, "r2") == 0)return 2;
	else if (strcmp(word, "r3") == 0)return 3;
	else if (strcmp(word, "r4") == 0)return 4;
	else if (strcmp(word, "r5") == 0)return 5;
	else if (strcmp(word, "r6") == 0)return 6;
	else if (strcmp(word, "r7") == 0)return 7;
	else return -1;
}

/*Checks if the word is a label, if so checks if the label name is correct*/
int isLable(char * word){
	if((isRegister(word) != -1) || (isInstruction(word) != -1) || word[0] < 'A' || word[0] > 'z' || strlen(word) > 31)
		return -1;
	while(*word != '\0'){
		if(!isalnum(*word))
			return -1;
		word++;
	}
	return 1;
}

/*A function that accepts a word (operand) and returns the number according to the sorting method*/
int chackOperandMethod(char * word){
	if(isRegister(word) != -1)/*in case of a register*/
		return 3;
	else if(isLable(word)==1)/*in case of a label*/
		return 1;
	else if(word[0] == '#'){/*in case of a #*/
		word++;
		if(word[0] == '+' || word[0] == '-')/*Checking what if the number is negative or positive*/
			word++;
		while (*word) {/*A loop that goes through the characters until the end of the word*/
        		if (!isdigit(*word))/*Checking that the characters are numbers*/
           			return -1;
       			word++;
    		}
		return 0;
	}
	else if(word[0] == '*' && isRegister(++word) != -1){/*in case the wors is in address to a register*/
		return 2;
	}
	else
		return -1;
}

/*Update the operand in the data structure*/
void updateOperand(char * currWord, int methodType, struct dataLine *dataLine, int i){
	if(methodType == 0){/*Update in a data structure whose operand type is immediate*/
		dataLine->lineOptions.instruction.operands[i].operandType = immed;
		dataLine->lineOptions.instruction.operands[i].operandValue.immed = atoi(++currWord);
	}
	else if(methodType == 1){/*Update in a data structure whose operand type is a lable*/
		dataLine->lineOptions.instruction.operands[i].operandType = lable;
        strcpy(dataLine->lineOptions.instruction.operands[i].operandValue.lable, currWord);
	}
	else if(methodType == 2){/*Update in a data structure whose operand type is a address register*/
		dataLine->lineOptions.instruction.operands[i].operandType = addressRg;
		dataLine->lineOptions.instruction.operands[i].operandValue.reg = isRegister(++currWord);
	}
	else{/*Update in a data structure whose operand type is a register*/
		dataLine->lineOptions.instruction.operands[i].operandType = registerOP;
		dataLine->lineOptions.instruction.operands[i].operandValue.reg = isRegister(currWord);
	}
}

/*A function that checks the correctness of the operand*/
void checkOperand(char **line, int commandNum, struct dataLine *dataLine){
	char currWord[80];
	char* temp;
	int methodType = 0;
	CHECK_ALLOC(currWord);
	temp = findWord(line);
	strcpy(currWord, temp);
	free(temp);
	if(extraText(line)){
		strcpy(dataLine->error, "Extra text at the end of the command");
		return;
	}
	methodType = chackOperandMethod(currWord);
	if(methodType == 1 || methodType == 2){/*In case the operand is according to method 1 or 2*/
		updateOperand(currWord, methodType, dataLine,0);
		return;
	}
	if(methodType == 3){/*In case the operand is type 3*/
		if((commandNum > 4 && commandNum < 9) || commandNum == 11 || commandNum == 12 ){
			 updateOperand(currWord, methodType, dataLine,0);
			 return;
		}
	}
	if(methodType == 0 && commandNum == 12){/*In case the operand is type 0*/
		updateOperand(currWord, methodType, dataLine,0);
		return;
	} 
	strcpy(dataLine->error, "Invalid operand");
	return;
}

/*A function that checks the correctness of the operands*/
void checkOperands(char **line, int commandNum, struct dataLine *dataLine){
	char currWord[80];
	char* temp;
	int methodType = 0;
	int numOfComma = 0;
	temp = findWord(line);
	strcpy(currWord, temp);
	free(temp);
	methodType = chackOperandMethod(currWord);
	if(commandNum == 4 && methodType != 1){
		strcpy(dataLine->error, "Invalid operand");
		return;
	}
	else if(methodType == -1){    
		strcpy(dataLine->error, "Invalid operand");
		return;
	}
	else{
		updateOperand(currWord, methodType, dataLine,0);
	}
	numOfComma = isComma(line);
	if(numOfComma == 0){
		strcpy(dataLine->error, "missing comma");
		return;
	}
	numOfComma += isComma(line);
	if(numOfComma == 2){
		strcpy(dataLine->error, "extra comma");
		return;
	}
	temp = findWord(line);
	strcpy(currWord, temp);
	free(temp);
	methodType = chackOperandMethod(currWord);
	if(extraText(line)){
		strcpy(dataLine->error, "Extra text at the end of the command");
		return;
	}
	if(methodType != -1 && commandNum == 1){/*in case of a correct command*/
		updateOperand(currWord, methodType, dataLine,1);
	}
	else if(methodType > 0){/*in case of a correct command*/
		updateOperand(currWord, methodType, dataLine,1);
	}
	else{
		strcpy(dataLine->error, "Invalid operand");
		return;
	}
}
/*A function that checks for a correct string*/
void checkString(char **line, struct dataLine *dataLine){
	char * ptrTemp; 
	int countChars = 0;
	checkSpace(line);
	ptrTemp = *line;
	(*line)++;      
    if(ptrTemp[0] == '"'){/*Checking that the first character is "*/
        ptrTemp++; 
		while(*ptrTemp != '\0' && *ptrTemp != '\n' ){
			countChars++;
		    if(*ptrTemp == '"'){/*In case you got to the second ""*/
		    	ptrTemp++;
			    if(extraText(&ptrTemp)){/*In case there is text after "*/
					strcpy(dataLine->error,"Extra text after the end of string");
					return;
				}
		        else{/*In case there is no text after " - Enter the STRING into the data structure*/
					dataLine->lineOptions.directive.directiveValue.string = (char *)malloc(strlen(*line) + 1);
		        	CHECK_ALLOC(dataLine->lineOptions.directive.directiveValue.string);
		       		strncpy(dataLine->lineOptions.directive.directiveValue.string, *line, countChars-1);
					dataLine->lineOptions.directive.directiveValue.string[countChars - 1] = '\0';
		        	return;
		        }
		    }
		    ptrTemp++;
		}
	}   
    strcpy(dataLine->error,"Invalid string");
    return;
}
/*A function that checks for entry or extern*/
void checkEntryAndExtern(char **line, struct dataLine *dataLine){
    	char currWord[80];
		char* temp;
    	temp = findWord(line);
		strcpy(currWord, temp);
		free(temp);
    	if(isLable(currWord) == 1){
        	if(extraText(line)){
            		strcpy(dataLine->error,"extra text in the end of a command");
            		return;
        	}
            strcpy(dataLine->lineOptions.directive.directiveValue.lable, currWord); 
            return;
    	}
    	strcpy(dataLine->error,"Invalid lable");	
    	return;
}
/*A function that checks for a correct int array*/
void checkData(char **line, struct dataLine *dataLine){
	int numOfComma = 0;
	int i = 0;
	int j = 0;
	int index = 0;
	int len = strlen(*line);
	char currWord[11];
	char* temp;
	while(i <= len){
		if((*line)[i] == ',')
			numOfComma++;
		i++;
	}
	dataLine->lineOptions.directive.directiveValue.numbers = (int *)malloc((numOfComma + 1)*sizeof(int));
	CHECK_ALLOC(dataLine->lineOptions.directive.directiveValue.numbers);
	while(**line){
		j = 0;
		temp = findWord(line);
		strcpy(currWord, temp);
		free(temp);
		if(*currWord == '+' || *currWord == '-')/*Checking what if the number is negative or positive*/
			j++;
		for (; j < (int)strlen(currWord); j++) {
            		if (!isdigit(currWord[j])) {/*Checking that the characters are  not numbers*/
                		strcpy(dataLine->error,"Not a number");
                		return;
            		}
        	}
        	dataLine->lineOptions.directive.directiveValue.numbers[index] = atoi(currWord);/*Convert the string to an integer and put in the data structer */
		dataLine->lineOptions.directive.countNumbers++;
		if(!(isComma(line))){
			if(extraText(line)){
				strcpy(dataLine->error,"Missing comma");
                		return;
			}
			return;
		}
		if(isComma(line)){
			strcpy(dataLine->error,"Extra comma");
                	return;
		}
		if(!(extraText(line))){
			strcpy(dataLine->error,"Unnecessary comma");
                	return;
		}
		index++;
	}
	strcpy(dataLine->error,"Invalid data");	
    	return;
}
/*A function that searches for a symbol*/
struct symbol * searchSymbol(struct symbol * symbolTable, int symbolCount, char * symbolName){
	int i;
	for(i = 0; i < symbolCount; i++ ){
		if(strcmp(symbolName, symbolTable[i].symbolName)==0){
			return &symbolTable[i];
		}
	}
	return NULL;
}
/*A function that searches for in external symbol*/
struct external * searchExternal(struct external * externalTable, int externalCount, char * externalName){
	int i;
	for(i = 0; i < externalCount; i++ ){
		if(strcmp(externalName, externalTable[i].externalName)==0){
			return &externalTable[i];
		}
	}
	return NULL;
}