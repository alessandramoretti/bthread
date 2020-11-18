all: bthread_test

bthread_test: bthread_test.c bthread.c tqueue.c
	gcc -fno-stack-protector -o bthread_test bthread.c bthread_test.c tqueue.c