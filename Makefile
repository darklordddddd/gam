
all: 	
	gcc -o xor xorator.c -fsanitize=address 
	gcc -o gen generator.c -fsanitize=address 

