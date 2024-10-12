#include "functions.h"
#define MAX_MACRO_NAME_LEN 30

char* pre_processor(char* file_name) {
	FILE* am_file;
	FILE* as_file;
	char* file_name_as = (char*)malloc(strlen(file_name) + 4);/*The name of the new file*/
	char* file_name_am = (char*)malloc(strlen(file_name) + 4);/*The name of the new file*/
	char line[MAX_LINE_LEN];
	char* copy_line_address;
	char* copy_line = (char*)malloc(MAX_LINE_LEN);/*The name of the new file*/
	char one_word[MAX_LINE_LEN];/*A single word from the line*/
	Macro* arr_macro = NULL;
	Macro* temp_arr_macro;
	char* temp_lines;
	char* temp_one_word;
	int arr_length = 0;
	int count_lines = 0;
	int new_macro_flag = 0;
	int flagError = 0;
	int i;
	CHECK_ALLOC(copy_line);
	CHECK_ALLOC(file_name_am);
	CHECK_ALLOC(file_name_as);
	strcpy(file_name_am, file_name);
	strcat(file_name_am, ".am");
	strcpy(file_name_as, file_name);
	strcat(file_name_as, ".as");
	am_file = fopen(file_name_am, "w");/*opening the file*/
	as_file = fopen(file_name_as, "r");/*opening the file*/
	free(file_name_as);
	if (!am_file || !as_file) {
		free(file_name_am);
		free(copy_line);
		if (as_file)
		{
			fclose(as_file);
		}
		if (am_file)
		{
			fclose(am_file);
		}
		return NULL;
	}
	copy_line_address = copy_line;
	while (fgets(line, MAX_LINE_LEN, as_file) != NULL) {
		copy_line = copy_line_address;
		strcpy(copy_line, line);
		temp_one_word = findWord(&copy_line);
		strcpy(one_word, temp_one_word);/*extracting the first word*/
		free(temp_one_word);
		if (find_macro_name(arr_length, arr_macro, one_word, am_file) == 1)/*Checking whether the macro exists in the data structure*/
			continue;
		else if (strcmp(one_word, "macr") == 0) {/*If we have a new macro we will put it in a data structure*/
			new_macro_flag = 1;
			temp_one_word = findWord(&copy_line);
			strcpy(one_word, temp_one_word);
			free(temp_one_word);
			if (find_macro_name(arr_length, arr_macro, one_word, am_file) == 1) {
				printf("existing macro\n");
				flagError = 1;
				continue;
			}
			else if (strcmp(one_word, "macr") == 0) {
				printf("Invalide macro name\n");
				flagError = 1;
				continue;
			}
			else if (extraText(&copy_line)) {/*Correctness check that there are no unnecessary characters*/
				printf("Extra text after define macro\n");
				flagError = 1;
				continue;
			}
			else if (isInstruction(one_word) != (-1) || isRegister(one_word) != (-1)) {/*Checking if the macro name is correct*/
				printf("Invalide macro name\n");
				flagError = 1;
				continue;
			}
			else if (arr_length == 0) {
				arr_macro = (Macro*)malloc(sizeof(Macro));
				arr_macro->lines = NULL;
				/*CHECK_ALLOC(arr_macro);*/
			}
			else {
				temp_arr_macro = (Macro*)realloc(arr_macro, (arr_length + 1) * sizeof(Macro));
				if (temp_arr_macro == NULL) {
					fprintf(stderr, "No memory\n");
					free(arr_macro);
					return NULL;
				}
				arr_macro = temp_arr_macro;
				arr_macro[arr_length].lines = NULL;
			}
			arr_macro[arr_length].macro_name = (char*)malloc(strlen(one_word) + 1);
			CHECK_ALLOC(arr_macro[arr_length].macro_name);
			strcpy(arr_macro[arr_length].macro_name, one_word);
			arr_length++;
			continue;
		}
		else if (strcmp(one_word, "endmacr") == 0) {/*If the line is the end of a macro*/
			if (extraText(&copy_line)) {/*Correctness check that there are no unnecessary characters*/
				printf("Extra text after end macro\n");
				flagError = 1;
			}
			new_macro_flag = 0;
			count_lines = 0;
			continue;
		}
		else {
			/*In the event that the line is related to a macro definition - we will enter a data structure*/
			if (new_macro_flag) {
				if (count_lines == 0) {
					if (arr_macro == NULL)
					{
						continue;
					}
					if (arr_macro[arr_length - 1].lines != NULL)
					{
						free(arr_macro[arr_length - 1].lines);
					}
					arr_macro[arr_length - 1].lines = (char*)malloc((strlen(line) + 1));
					CHECK_ALLOC(arr_macro[arr_length - 1].lines);
					strcpy(arr_macro[arr_length - 1].lines, line);
					count_lines++;
				}
				else {
					temp_lines = (char*)realloc(arr_macro[arr_length - 1].lines,
						strlen(arr_macro[arr_length - 1].lines) + strlen(line) + 1);
					if (temp_lines == NULL) {
						fprintf(stderr, "No memory\n");
						free(arr_macro[arr_length - 1].lines);
						return NULL;
					}
					arr_macro[arr_length - 1].lines = temp_lines;
					strcat(arr_macro[arr_length - 1].lines, line);
				}
			}
			/*Print the line to the new file*/
			else
				fprintf(am_file, "%s", line);
		}

	}
	free(copy_line_address);
	fclose(as_file);
	fclose(am_file);
	for (i = 0; i < arr_length; i++)
	{
		free(arr_macro[i].lines);
		free(arr_macro[i].macro_name);
	}
	free(arr_macro);
	if (flagError) {
		remove(file_name_am);
		free(file_name_am);
		return NULL;
	}
	return file_name_am;
}