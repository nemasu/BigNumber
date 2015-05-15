CC=g++
CCOPTS=-std=c++11

all: release

release: CCOPTS += -O3
release: clean test_BigUInt run clean

debug: CCOPTS += -g
debug: test_BigUInt

test_BigUInt: BigUInt_test.cpp include/BigUInt.h
	${CC} ${CCOPTS} BigUInt_test.cpp -o test_BigUInt

run:
	bash -c 'time ./test_BigUInt'

clean:
	rm -rf test_BigUInt
