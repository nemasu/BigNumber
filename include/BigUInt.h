#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

#define MAX_LIMIT (uint64_t)1000000000
#define MAX_DIGIT 9

using std::vector;
using std::string;

class BigUInt {
	public:

		BigUInt() {

		}

		BigUInt( const char * in ) {
			string strIn(in);
			*this = strIn;
		}

		BigUInt( string in ) {
			long long length = in.length();
		
			while( length != 0 ) {
				if( length - MAX_DIGIT > 0 ) {
					value.push_back(atol( in.substr( length - MAX_DIGIT, MAX_DIGIT ).c_str()));
				} else {
					value.push_back(atol( in.substr( 0, length - MAX_DIGIT + 9 ).c_str()));
					break;
				}
				length -= MAX_DIGIT;
			}
		}
	
		BigUInt( vector<uint64_t> in ) {
			value = in;
		}
		
		BigUInt( vector<uint64_t>::iterator in1, vector<uint64_t>::iterator in2 ) {
			value = vector<uint64_t>(in1, in2);
			if (value.size() == 0 ) {
				value.push_back(0);
			}
		}

		BigUInt( uint64_t in ) {
			if( in > 999999999 && in < 1000000000000000000 ) {
				value.push_back( in % MAX_LIMIT );
				value.push_back( in / MAX_LIMIT );
			} else if ( in > 1000000000000000000 ) {
				value.push_back( in % MAX_LIMIT );
				value.push_back( (in / MAX_LIMIT) % MAX_LIMIT );
				value.push_back( in / 1000000000000000000 );
			}
			else {
				value.push_back( in );
			}
		}

		size_t
		size() {
			return value.size();
		}

		uint64_t
		toUint64() {
			uint64_t ret = 0;
			if( value.size() > 2 ) {
				std::cerr << "Error: Can't convert to uint64, value too large." << std::endl; 
				return 0;
			}

			ret = value[1] * MAX_LIMIT;
			ret += value[0];

			return ret;
		}

		
	private:
		//Stores value in reverse
		vector<uint64_t> value;
		
		void insertAt( size_t location, uint32_t a ) {
			value.insert(value.begin() + location, a);
		}
		
		static BigUInt
		Mul_u32int( BigUInt &a, uint32_t b ) {
			BigUInt ret;
			uint64_t carry = 0;	

			vector<uint64_t>::iterator beg = a.value.begin();
			vector<uint64_t>::iterator end = a.value.end();
			while(beg != end) {
				uint64_t av = *beg;
				uint64_t value = av * b + carry;

				ret.value.push_back(value % MAX_LIMIT);
				carry = value / MAX_LIMIT;

				beg++;
			}

			if( carry ) {
				ret.value.push_back(carry);
			}
			return ret;

		}
		
		static uint64_t	
		pad( BigUInt &a, BigUInt &b ) {
			if ( a.value.size() > b.value.size() ) {
				int diff = (a.value.size() - b.value.size());
				for ( int i = 0; i < diff; ++i ) {
					b.value.push_back(0);
				}
			} else {
				int diff = (b.value.size() - a.value.size());
				for ( int i = 0; i < diff; ++i ) {
					a.value.push_back(0);
				}
			}
			return a.value.size();
		}

		static void
		unpad( BigUInt &a, BigUInt &b ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
			
			while(b.value.back() == 0 && b.value.size() != 1) {
				b.value.pop_back();
			}
		}
		
		static void
		unpad( BigUInt &a ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
		}

		static BigUInt
		karatsuba( BigUInt &a, BigUInt &b ) {
			if( a.size() == 0 || b.size() == 0 ) {
				std::cerr << "Error: karatsuba - a value has no contents." << std::endl;
			}
			
			if( a.size() == 1 || b.size() == 1 ) {
				return a * b;
			}
			
			BigUInt ret;

			uint64_t m = pad(a, b);
			uint64_t m2 = m/2;

			ret.value.reserve(m*2);
			
			BigUInt  low1(a.value.begin()     , a.value.begin() + m2);
			BigUInt high1(a.value.begin() + m2, a.value.end());
			
			BigUInt  low2(b.value.begin()     , b.value.begin() + m2);
			BigUInt high2(b.value.begin() + m2, b.value.end());

			BigUInt z0 = karatsuba(low1, low2);
		
			BigUInt lowhigh1 = low1 + high1;
			BigUInt lowhigh2 = low2 + high2;
			BigUInt z1 = karatsuba(lowhigh1, lowhigh2);
			
				
			BigUInt z2 = karatsuba(high1, high2);
			
			BigUInt zdiff = z1 - z2;
			zdiff = zdiff - z0;

			for (int i = 0; i < m2 * 2; i++) {
				z2.insertAt(0, 0);
			}

			for (int i = 0; i < m2; i++) {
				zdiff.insertAt(0, 0);
			}
		
			ret = z2 + zdiff;
			ret = ret + z0;

			unpad(a, b);
			unpad(ret);

			return ret;

		}
	friend bool operator==( BigUInt &a, BigUInt &b );
	friend bool operator!=( BigUInt &a, BigUInt &b );
	friend bool operator< (BigUInt &a, uint32_t b);
	friend bool operator> (BigUInt &a, uint32_t b);
	friend std::ostream& operator <<(std::ostream& stream, const BigUInt& a);
	friend BigUInt operator+( BigUInt &a, BigUInt &b );
	friend BigUInt operator-( BigUInt &a, BigUInt &b );
	friend BigUInt operator-( BigUInt &a, uint32_t b );
	friend BigUInt operator*( BigUInt &a, BigUInt &b );
	friend BigUInt operator*( BigUInt &a, uint32_t b );
	friend BigUInt operator^( BigUInt &a, BigUInt &b );
};

BigUInt
operator^( BigUInt &a, BigUInt &b ) {
	BigUInt ret = a;
	BigUInt count(b-1);
	while( count > 0  ) {
		ret = ret * a;
		count = count - 1;
	}

	return ret;
}

bool
operator> (BigUInt &a, uint32_t b) {
	return a.value.size() == 1 ? a.value[0] > b : false;
}

bool
operator< (BigUInt &a, uint32_t b) {
	return a.value.size() == 1 ? a.value[0] < b : false;
}

BigUInt
operator-( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator-(a, b);
}
	
BigUInt
operator-( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	uint64_t size = BigUInt::pad(a, b);
	ret.value.reserve(size);
	uint64_t i = 0;
	bool borrow = false;
	int64_t result  = 0;

	for (; i < size; i++) {
		result = a.value[i] - b.value[i];
		if( borrow ) {
			result -= 1;
			borrow = false;
		}

		if( result <  0 ) {
			result = MAX_LIMIT + result;
			borrow = true;
		}

		ret.insertAt(i, result % MAX_LIMIT);
	}

	if( borrow ) {
		ret.insertAt(i, 
					ret.value[i] - 1
				);
	}

	BigUInt::unpad(a, b);
	return ret;
}

BigUInt
operator+( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	ret.value.reserve(a.value.size() + b.value.size() + 1);
	uint64_t size = BigUInt::pad(a, b);
	uint64_t carry = 0;
	uint64_t i = 0;
	uint64_t result  = 0;

	for (; i < size; i++) {
		result = b.value[i] + a.value[i] + carry;
		ret.insertAt(i, result % MAX_LIMIT);
		carry = result / MAX_LIMIT;
	}

	if ( carry != 0 ) {
		ret.insertAt(i, carry);
	}

	BigUInt::unpad(a, b);
	return ret;
}

std::ostream&
operator <<(std::ostream& stream, const BigUInt& a) {
	auto rbeg = a.value.rbegin();
	auto rend = a.value.rend();

	while( rbeg != rend ) {
		if (*rbeg != 0) {
			stream << *rbeg;
		} else {
			stream << "000000000";
		}
		rbeg++;
	}

	return stream;
}

BigUInt
operator*( BigUInt &a, BigUInt &b ) {
	if( (a.value.size() == 1 && a.value[0] == 0) || (b.value.size() == 1 && b.value[0] == 0) ) {
		return (uint64_t)0;			
	} else if (a.value.size() == 1 && b.value.size() == 1 ) {
		return BigUInt((uint64_t)(a.value[0] * b.value[0]));
	} else if (  a.value.size() == 1 || b.value.size() == 1 ) {
		if ( a.value.size() == 1 ) {
			return BigUInt::Mul_u32int( b, a.value[0]);
		} else {
			return BigUInt::Mul_u32int( a, b.value[0]);
		}
	} else 	{
		return BigUInt::karatsuba(a, b);
	}
}

BigUInt
operator*( BigUInt &a, uint32_t buint ) {
	BigUInt b(buint);
	return operator*(a, b);
}

bool
operator!=( BigUInt &a, BigUInt &b ) {
	return !operator==(a, b);
}
bool
operator==( BigUInt &a, BigUInt &b ) {
	if( a.size() != b.size() ){
		return false;
	}

	for( int i = 0; i < a.size(); ++i ) {
		if( a.value[i] != b.value[i] ) {
			return false;
		}
	}
	return true;
}
