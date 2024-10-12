#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "twoPasses.h"

extern char * pre_processor(char * file_name);
extern int firstPass(struct translationUnit * program, char * amFileName, FILE * amFile);
extern int secondPass(struct translationUnit * program, char * amFileName, FILE * amFile);
extern void printObEntExt(struct translationUnit * program, char * fileName);

int main(int argc, char ** argv){
	int i, j;
	char * am_file_name;
	FILE * am_file;
	struct translationUnit program = {0};
	for(i = 1; i < argc; i++){/*A loop that goes through all the files*/
		am_file_name = pre_processor(argv[i]);
		memset(&program, 0, sizeof(struct translationUnit));
		if(am_file_name){/*In case there is in am file name */
			am_file = fopen(am_file_name, "r");
			if(am_file){/*/*In case an am file opened */
				if(!firstPass(&program, am_file_name, am_file)){
					rewind(am_file);
					if(!secondPass(&program, am_file_name, am_file)){
						printObEntExt(&program, argv[i]);
					}
				}
				free(program.imageCode);
				free(program.imageData);
				free(program.symbolTable);
				for (j = 0; j < program.externalCount; j++) {
					free(program.externals[j].externalAddresses);
				}
				free(program.externals);
				free(program.entries);
				fclose(am_file);
			}
			free(am_file_name);
		}
	}
	return 0;
}
