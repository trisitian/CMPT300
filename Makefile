all: cshell.c
	gcc -Wall -o cshell cshell.c

clean: 
	$(RM) cshell