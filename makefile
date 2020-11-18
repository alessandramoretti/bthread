all: bthread_test

bthread_test: bthread_test.o libbthread.a
		gcc -lm -o bthread_test bthread_test.o -L. -lbthread  -fno-stack-protector

tqueue_test: tqueue_test.o libbthread.a
		gcc -lm -o tqueue_test tqueue_test.o -L. -lbthread -fno-stack-protector

bthread_test.o: bthread_test.c
		gcc -O -c bthread_test.c -fno-stack-protector

test.o: tqueue_test.c
		gcc -O -c tqueue_test.c -fno-stack-protector

bthread.o: bthread.c bthread.h
		gcc -O -c bthread.c -fno-stack-protector

tqueue.o: tqueue.c tqueue.h
		gcc -O -c tqueue.c -fno-stack-protector

libbthread.a: bthread.o tqueue.o
		ar rcs libbthread.a bthread.o tqueue.o

