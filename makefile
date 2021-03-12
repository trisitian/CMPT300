all: lets-talk.c
	gcc -Wall -o lets-talk lets-talk.c

clean: 
	$(RM) lets-talk