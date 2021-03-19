all: lets-talk.c list.o list.h
	gcc -Wall -g -o lets-talk list.o lets-talk.c -lpthread

clean: 
	$(RM) lets-talk