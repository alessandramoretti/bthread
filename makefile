all: tqueue_test

tqueue_test: tqueue_test.o libbthread.a
		gcc -lm -o tqueue_test tqueue_test.o -L. -lbthread

test.o: tqueue_test.c
		gcc -O -c tqueue_test.c

bthread.o: bthread.c bthread.h
		gcc -O -c bthread.c

tqueue.o: tqueue.c tqueue.h
		gcc -O -c tqueue.c

libbthread.a: bthread.o tqueue.o
		ar rcs libbthread.a bthread.o tqueue.o

