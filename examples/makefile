CC = gcc
CFLAGS = -g -fno-stack-protector -w -DTRACING #remove DTRACING if you do not want to use the tracer mechanism
SRC := ../src
BIN := ./bin/
MKDIR_P = mkdir -p

all: directories matrix_cube readers_writers_3 readers_writers_2 readers_writers_1 sleeping_barber producer_consumer_cond producer_consumer philosopher

directories:
		${MKDIR_P} ${BIN}

matrix_cube: matrix_cube.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

readers_writers_3: readers_writers_3.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

readers_writers_2: readers_writers_2.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

readers_writers_1: readers_writers_1.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

sleeping_barber: sleeping_barber.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

producer_consumer_cond: producer_consumer_cond.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

producer_consumer: producer_consumer.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

philosopher: philosopher.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o ${BIN}$@ $?

clean:
	rm -r ${BIN}