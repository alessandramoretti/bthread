all: bthread_test tqueue_test

bthread_test: bthread_test.c bthread.c tqueue.c tmutex.c
	gcc -g -fno-stack-protector -o bthread_test bthread.c bthread_test.c tqueue.c tmutex.c

tqueue_test: tqueue_test.c bthread.c tqueue.c
	gcc -fno-stack-protector -o tqueue_test bthread.c tqueue_test.c tqueue.c

clean:
	rm bthread_test tqueue_test *.o