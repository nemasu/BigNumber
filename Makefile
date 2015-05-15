all: test_BigUInt

test_BigUInt: BigUInt_test.cpp include/BigUInt.h
	g++ -g -std=c++11 BigUInt_test.cpp -o test_BigUInt

clean:
	rm -rf test_BigUInt
