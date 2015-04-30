#include "include/BigUInt.h"

using std::string;
using std::cout;
using std::endl;

void
check( string test, BigUInt &value, string answer ) {
	BigUInt correct(answer);
	cout << test << ": ";
	if( value == correct ) {
		cout << "PASS";
	} else {
		cout << "FAIL; Got: " << value;;

	}
	cout << endl;
}

void
test_exp() {
	
	BigUInt x1("458752045245");
	BigUInt x2("12");
	BigUInt total = x1 ^ x2;

	check("test_exp", total, "86883270736994799311980968740310906566810752413024134167950461737851897861230188355827751449141728642574950576958130475206898658789306640625");
}

void
test_add() {
	
	BigUInt x1(131185607);
	BigUInt x2(592432186);
	BigUInt x3(840600773);
	BigUInt x4(651266700);
	BigUInt x5(172464117);
	BigUInt x6(728043153);

	BigUInt total = x1 + x2;
	total = total + x3;
	total = total + x4;
	total = total + x5;
	total = total + x6;

	check("test_add", total, "3115992536");
}

void
test_sub() {

	BigUInt x1("1208609638235972680");
	BigUInt x2("77718575926746902");
	
	BigUInt total = x1 - x2;

	check("test_sub", total, "1130891062309225778");

}

void
test_easymul() {
	BigUInt x1("12345678901234567890");
	uint32_t b = 999999999;

	x1 = x1 * b;

	check("test_easymul", x1, "12345678888888888988765432110");
}

void
test_hardmul() {

	BigUInt x1("131185607840600773");
	BigUInt x2("592432186651266700");
	BigUInt x3("172464117728043153");

	BigUInt total = x1 * x2;
	total = total * x3;

	check("test_hardmul", total, "13403665728908073767072787772731749128550164362642300");
}

int
main( int argv, char **argc ) {

	test_add();
	test_sub();
	test_easymul();
	test_hardmul();
	test_exp();

	return 0;
}
