all: lets-talk.c list.o list.h

	gcc -Wall -g -o lets-talk list.o lets-talk.c -lpthread
valgrind:
	valgrind --leak-check=full ./lets-talk 6000 localhost 6001


clean: 
	$(RM) lets-talk