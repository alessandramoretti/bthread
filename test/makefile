CC = gcc
CFLAGS = -g -fno-stack-protector -w -DTRACING #remove DTRACING if you do not want to use the tracer mechanism
SRC := ../src
BIN := ./bin/
MKDIR_P = mkdir -p

all: directories bthread_test tqueue_test

directories:
		${MKDIR_P} ${BIN}

bthread_test: bthread_test.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o $(BIN)$@ $?

tqueue_test: tqueue_test.c $(SRC)/tqueue.c
	$(CC) $(CFLAGS) -o $(BIN)$@ $?

clean:
	rm -r $(BIN)