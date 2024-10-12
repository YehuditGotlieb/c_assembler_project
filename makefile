run: main.c preprocessor.o functions.o front.o twoPasses.o createFiles.o
	gcc  -ansi -pedantic -Wall main.c preprocessor.o functions.o front.o twoPasses.o createFiles.o -o run


preprocessor.o: preprocessor.c functions.h
	gcc -c -ansi -pedantic -Wall preprocessor.c -o preprocessor.o

functions.o: functions.c functions.h
	gcc -c -ansi -pedantic -Wall functions.c -o functions.o

front.o: front.c front.h functions.h
	gcc -c -ansi -pedantic -Wall front.c -o front.o

twoPasses.o: twoPasses.c twoPasses.h front.h functions.h
	gcc -c -ansi -pedantic -Wall twoPasses.c -o twoPasses.o	

createFiles.o: createFiles.c twoPasses.h functions.h
	gcc -c -ansi -pedantic -Wall createFiles.c -o createFiles.o	

