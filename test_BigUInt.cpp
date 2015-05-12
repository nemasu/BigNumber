#include "include/BigUInt.h"
#include <sstream>

using std::stringstream;
using std::string;
using std::cout;
using std::endl;

void
check( string test, BigUInt value, uint64_t correct ) {
	BigUInt answer(correct);
	cout << test << ": ";
	if( value == answer ) {
		cout << "PASS";
	} else {
		cout << "FAIL; Got: " << value << ", expected: " << answer;

	}
	cout << endl;
}
void
check( string test, bool value, bool answer ) {
	cout << test << ": ";
	if( value == answer ) {
		cout << "PASS";
	} else {
		cout << "FAIL; Got: " << value << ", expected: " << answer;

	}
	cout << endl;
}

void
check( string test, BigUInt &value, string answer ) {
	BigUInt correct(answer);
	cout << test << ": ";
	if( value == correct ) {
		cout << "PASS";
	} else {
		cout << "FAIL; Got: " << value << ", expected: " << answer;

	}
	cout << endl;
}

void
test_hardmodexp() {
	BigUInt base("35052111338673026690212423937053328511880760811579981620642802346685810623109850235943049080973386241113784040794704193978215378499765413083646438784740952306932534945195080183861574225226218879827232453912820596886440377536082465681750074417459151485407445862511023472235560823053497791518928820272257787786");
	BigUInt exp("65537");
	BigUInt mod("145906768007583323230186939349070635292401872375357164399581871019873438799005358938369571402670149802121818086292467422828157022922076746906543401224889672472407926969987100581290103199317858753663710862357656510507883714297115637342788911463535102712032765166518411726859837988672111837205085526346618740053");

	BigUInt result = BigUInt::ModExp(base, exp, mod);
	check("test_hardmodexp", result, "1976620216402300889624482718775150");
}

void
test_modexp() {
	BigUInt base("2790");
	BigUInt exp("2753");
	BigUInt mod("3233");

	BigUInt result = BigUInt::ModExp(base, exp, mod);
	check("test_modexp", result, "65");
}

void
test_hardmod() {
	BigUInt x1("24249719036845860583094558314648139249167888395517390568299305365769030613549284144697706088226150109986959452307811445423198529027126224695685433864734303042258615028046447228030937748950777759487505322689442073931693992759503614350050494986232639272291811754601475617120506218185436132378144567259565024712100104883204789381429412979987271264534260030874940546440909231696843595318548263233799464848524519406551940414027539280105087352774101302305215135018820418356300750870268858500673410111783067427981449262321507925297395206464355749227694300543396226024886035494624124460050095940558599637849773202190748811237");
	BigUInt x2("242497190368458693992759503614350050494986232639272291811754601475617120506218185436132378144567259565024712100104883204789381429412979987271264534260030874940546440909231696843595318548263233799464848524519406551940414027539280105087352774101302305215135018820418356300750870268858500673410111783067427981449262321507925297395206464355749227694300543396226024886035494624124460050095940558599637849773202190748811237");

	BigUInt result = x1 % x2;
	check("test_hardmod", result, "124574119362591273342709720687785452463792606600929460883585452941285244492332920210576497996845490818648542448397160203745732418642991893576039320012555887683425119038664138308192781459561878467585290555048445732032302592156515287484405689794382698048374418076208804448365718548360933315833311450995804375803317013765084680598257968138718904843676715887258685543765887242344973842510055156626514540358819614996380823");
}

void
test_mod() {
	BigUInt x1("12458987452475924952952524952452452452456294576245");
	BigUInt x2("4057280475247582274502475807");

	BigUInt result = x1 % x2;

	check("test_mod", result, "1434661386710805841329943680");
}

void
test_rsh() {
	BigUInt x1(0x99999999);

	BigUInt result = x1 >> 1;

	check("test_rsh", result, 0x4CCCCCCC);
}

void
test_lsh() {
	BigUInt x1(0x99999999);

	BigUInt result = x1 << 1;

	check("test_lsh", result, 0x133333332);
}


void
test_div() {
	BigUInt x1(100);
	BigUInt x2(50);

	BigUInt result = x1 / x2;

	check("test_div (half)", result, 2);

	x1 = 100;
	x2 = 100;
	result = x1 / x2;

	check("test_div (same)", result, 1);

	x1 = 123456789;
	x2 = 675;
	result = x1 / x2;

	check("test_div (odd)", result, 182898);

	uint64_t l = 2452466892468925684;
	uint64_t r = 458248598258;

	x1 = l;
	x2 = r;
	result = x1 / x2;

	check("test_div (large)", result, (uint64_t) l / r);

}

void
test_harddiv() {
	BigUInt x1("1234567899990274058280457");
	BigUInt x2("4057280475247582");

	BigUInt result = x1 / x2;

	check("test_harddiv", result, "304284583");
}

void
test_stringctor() {

	BigUInt x1 ("1234567890");
	stringstream ss1;
	ss1 << x1;
	check("test_stringctor", ss1.str() == "1234567890", true);

	BigUInt x2 ("12345678903");
	stringstream ss2;
	ss2 << x2;
	check("test_stringctor2", ss2.str() == "12345678903", true);


	BigUInt x3 ("123456789999027458280457");
	stringstream ss3;
	ss3 << x3;
	check("test_stringctor3", ss3.str() == "123456789999027458280457", true);
}

void
test_lt() {
	BigUInt x1("1234567899990274058280457");
	BigUInt x2("12345678999902740582804572");
	
	check("test_lt", x1 < x2, true);

	x1 = "1234567899990274058280456";
	x2 = "1234567899990274058280457";
	
	check("test_lt2", x1 < x2, true);

	x1 = "1234567899990274058280457";
	x2 = "2234567899990274058280457";
	
	check("test_lt3", x1 < x2, true);

	x1 = "123456789999027405828045700000";
	x2 = "1234567899990274058280457";
	
	check("test_lt4", x1 < x2, false);

	x1 = "1234567899990";
	x2 = "4057280475247582";
	
	check("test_lt5", x1 < x2, true);

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

	check("test_add", total, 3115992536);
	
	BigUInt total2((uint64_t)0);
	uint64_t r = 3115992536;
	uint64_t l = 7290;
	BigUInt toadd(r);
	for( uint64_t i= 0; i < l; ++i ) {
		total2 = total2 + toadd;
	}

	check("test_add2", total2, (uint64_t)r * l);
}

void
test_sub() {

	BigUInt x1(1208609638235972680);
	BigUInt x2(77718575926746902);
	
	BigUInt total = x1 - x2;

	check("test_sub", total, 1130891062309225778);
	
	x1 = 0x2208E9A51D0;
	x2 = 0x21578B97EBA;

	total = x1 - x2;
		
	check("test_sub2", total, 0xB15E0D316);

}

void
test_easymul() {
	BigUInt x1(240404546);
	uint32_t b = 341201;

	x1 = x1 * b;

	check("test_easymul", x1, 82026271499746);

	x1 = 1582474798;
	BigUInt x2 = 1445525157;

	x1 = x1 * x2;

	check("test_easymul2", x1, 2287507130827493286);

	x1 = 458248598258;
	x2 = 2;

	x1 = x1 * x2;
	
	check("test_easymul3", x1, 916497196516);

	x1 = 0x2208E9A51D0;
	x2 = 0x5;

	x1 = x1 * x2;
	
	check("test_easymul4", x1, 0xAA2C9039910);

	x1 = 12345678903;
	x2 = 16;

	x1 = x1 * x2;

	check("test_easymul5", x1, 197530862448);

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
	test_rsh();
	test_lsh();
	test_easymul();
	test_div();
	test_stringctor();
	test_hardmul();
	test_harddiv();
	test_exp();
	test_lt();
	test_mod();
	test_hardmod();
	test_modexp();
	test_hardmodexp();

	return 0;
}
