all: cshell.c
	gcc -Wall -o lets-talk lets-talk.c

clean: 
	$(RM) cshell