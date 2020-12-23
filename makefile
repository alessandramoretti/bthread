CC = gcc
CFLAGS = -g -fno-stack-protector -w

all: bthread_test tqueue_test matrix_cube readers_writers_3 readers_writers_2 readers_writers_1 sleeping_barber producer_consumer_cond producer_consumer philosopher

matrix_cube: matrix_cube.c bthread.c tqueue.c tbarrier.c
	$(CC) $(CFLAGS) -o matrix_cube matrix_cube.c tqueue.c bthread.c tbarrier.c

readers_writers_3: readers_writers_3.c bthread.c tqueue.c tsemaphore.c
	$(CC) $(CFLAGS) -o readers_writers_3 readers_writers_3.c tqueue.c bthread.c tsemaphore.c

readers_writers_2: readers_writers_2.c bthread.c tqueue.c tsemaphore.c
	$(CC) $(CFLAGS) -o readers_writers_2 readers_writers_2.c tqueue.c bthread.c tsemaphore.c

readers_writers_1: readers_writers_1.c bthread.c tqueue.c tsemaphore.c
	$(CC) $(CFLAGS) -o readers_writers_1 readers_writers_1.c tqueue.c bthread.c tsemaphore.c

sleeping_barber: sleeping_barber.c bthread.c tqueue.c tsemaphore.c tmutex.c
	$(CC) $(CFLAGS) -o sleeping_barber sleeping_barber.c tqueue.c bthread.c tsemaphore.c tmutex.c

producer_consumer_cond: producer_consumer_cond.c bthread.c tqueue.c tcondition.c tmutex.c
	$(CC) $(CFLAGS) -o producer_consumer_cond producer_consumer_cond.c tqueue.c bthread.c tcondition.c tmutex.c

producer_consumer: producer_consumer.c bthread.c tqueue.c tmutex.c tsemaphore.c
	$(CC) $(CFLAGS) -o producer_consumer producer_consumer.c tqueue.c bthread.c tmutex.c tsemaphore.c

philosopher: philosopher.c bthread.c tqueue.c tmutex.c tsemaphore.c
	$(CC) $(CFLAGS) -o philosopher philosopher.c tqueue.c bthread.c tmutex.c tsemaphore.c

bthread_test: bthread_test.c bthread.c tqueue.c tmutex.c tsemaphore.c tbarrier.c tcondition.c
	$(CC) $(CFLAGS) -o bthread_test bthread_test.c tqueue.c bthread.c tmutex.c tsemaphore.c tbarrier.c tcondition.c

tqueue_test: tqueue_test.c tqueue.c
	$(CC) $(CFLAGS) -o tqueue_test tqueue_test.c tqueue.c

clean:
	rm bthread_test matrix_cube tqueue_test readers_writers_1 readers_writers_2 readers_writers_3 sleeping_barber philosopher producer_consumer producer_consumer_cond *.o