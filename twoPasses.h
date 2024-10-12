#ifndef TWOPASSES_H
#define TWOPASSES_H
#include <stdio.h>
#include <string.h>
#define MAX_SYMBOL_LENGTH 32


/*Data structure for external labels*/
struct external {
	char externalName[MAX_SYMBOL_LENGTH];
	int * externalAddresses;
	int addressCount;
};

/*Data structure for unit translation*/
struct translationUnit {
	int * imageCode;
	int IC;
	int * imageData;
	int DC;
	struct symbol * symbolTable;
	int symbolCount;
	struct external * externals;
	int externalCount;
	struct symbol ** entries;
	int entriesCount;
};
/*Data structure for the symbol*/
struct symbol {
	char symbolName[MAX_SYMBOL_LENGTH];
	int address;
	enum{
		symbolExtern, symbolEntry,
		symbolCode, symbolData,
		symbolEntryCode, symbolEntryData
	}symbolType;
};
#endif
