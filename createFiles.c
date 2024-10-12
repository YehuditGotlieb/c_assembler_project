
#include "functions.h"
#include "twoPasses.h"
/*A function that opens files and prints values ​​to the appropriate file*/
void printObEntExt(struct translationUnit * program, char * fileName){
    char * obFileName = (char *)malloc(sizeof(fileName) + 4);
    FILE * obFile;
    char * entFileName = (char *)malloc(sizeof(fileName) + 5);
    FILE * entFile;
    char * extFileName = (char *)malloc(sizeof(fileName) + 5);
    FILE * extFile;
    int i;
    int j;
    CHECK_ALLOC(obFileName);
    CHECK_ALLOC(entFileName);
    CHECK_ALLOC(extFileName);
    strcpy(obFileName, fileName);
    strcat(obFileName, ".ob");
    obFile = fopen(obFileName, "w");
    free(obFileName);
    if(obFile){/*In case an OB file opened */
        fprintf(obFile, "   %d     %d  \n", program->IC , program->DC);/*prints IC and DC*/
        for(i=0;i<program->IC;i++){/*A loop that goes through and prints the addresses in 4-bit decimal base */
            fprintf(obFile, "  %04d  %d%d%d%d%d \n", i + 100, 
            (program->imageCode[i] >> 12) & 7,
            (program->imageCode[i] >> 9) & 7,
            (program->imageCode[i] >> 6) & 7,
            (program->imageCode[i] >> 3) & 7,
            (program->imageCode[i]) & 7);
        }
        for(i=0;i<program->DC;i++){/*A loop that goes through and prints the content of the word in 5-bit octal base*/
            fprintf(obFile, "  %04d  %d%d%d%d%d \n", program->IC + i + 100, 
            (program->imageData[i] >> 12) & 7,
            (program->imageData[i] >> 9) & 7,
            (program->imageData[i] >> 6) & 7,
            (program->imageData[i] >> 3) & 7,
            (program->imageData[i]) & 7);
        }
        fclose(obFile);
    }
    if(program->entriesCount > 0){/*In case there is at least one lable that is defined as an entry*/
        strcpy(entFileName, fileName);  
        strcat(entFileName, ".ent");
        entFile = fopen(entFileName, "w");
        if(entFile){/*In case an ENT file opened */
            for(i = 0; i < program->entriesCount; i++){/*A loop that goes through and prints the name of the symbol and its value in decimal base*/
                fprintf(entFile, "%s  %d \n", program->entries[i]->symbolName, program->entries[i]->address );
            }
            fclose(entFile);
        } 
    }
    
    if(program->externalCount > 0){/*In case there is at least one lable that is defined as an extern*/
        strcpy(extFileName, fileName);  
        strcat(extFileName, ".ext");
        extFile = fopen(extFileName, "w");
        if(extFile){/*In case an EXT file opened */
            for(i = 0; i < program->externalCount; i++){/*A loop that goes through and prints the name of the symbol and its address*/
                for(j = 0; j <  program->externals[i].addressCount; j++){/*A loop that goes through all the addresses that the same symbol has*/
                    fprintf(extFile, " %s %04d \n", program->externals[i].externalName, program->externals[i].externalAddresses[j]);
                }
            }
            fclose(extFile);
        } 
    }  
    free(entFileName);
    free(extFileName);
}
