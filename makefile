all: myls.c
	gcc -Wall -g -o myls myls.c
clean: 
	$(RM) myls