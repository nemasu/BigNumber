all: test_BigUInt

test_BigUInt: test_BigUInt.cpp
	g++ -g -std=c++11 test_BigUInt.cpp -o test_BigUInt

clean:
	rm -rf test_BigUInt
