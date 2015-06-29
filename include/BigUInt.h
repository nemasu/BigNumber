#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>

#define MAX_LIMIT (uint64_t)0x100000000
#define MAX_DIGIT_HEX 8
#define UINT64_HIGH 0xFFFFFFFF00000000
#define UINT64_LOW  0xFFFFFFFF

#define BARRETT_THRESHOLD 3

static const char DigitToHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

using std::vector;
using std::string;

class BigUInt {
	public:

		BigUInt() {

		}

		BigUInt( const char * in ) {
			string inStr(in);
			initFromString(inStr);
		}

		BigUInt( string in ) {
			initFromString(in);
		}
	
		BigUInt( vector<uint32_t> in ) {
			value = in;
		}
		
		BigUInt( vector<uint32_t>::iterator in1, vector<uint32_t>::iterator in2 ) {
			value = vector<uint32_t>(in1, in2);
			if (value.size() == 0 ) {
				append(0);
			}
		}

		BigUInt( uint64_t in ) {
			if( in >= MAX_LIMIT ) {
				uint64_t high = in & UINT64_HIGH;
				high = high >> 32;
				value.push_back( in & UINT64_LOW  );
				value.push_back( high );
			} else {
				append( in );
			}
		}
		
		
		size_t
		size() {
			return value.size();
		}

		uint64_t
		toUint64() {
			uint64_t ret = 0;
			if        ( value.size() == 1 ) {
				ret = value[0];
				return ret;
			} else if ( value.size() == 2 ) {
				ret |= value[1];
				ret =  ret << 32;
				ret |= value[0];
			} else {
				std::cerr << "Error: Can't convert to uint64, value too large." << std::endl; 
				return 0;
			}

			return ret;
		}

		static BigUInt
		DivideWithRemainder( BigUInt &N, BigUInt &D, BigUInt *pR ) {
			if( D == 0 ) {
				std::cerr << "Error: Divide by 0." << std::endl;
				BigUInt ret;
				return ret;
			}

			BigUInt ret;
			if( D == 2 && !pR ) {
				ret = N >> 1;
				return ret;
			}
			if( D == N ) {
				ret = 1;
				if( pR ) {
					*pR = (uint64_t) 0;
				}
				return ret;
			}

			if( N < D ) {
				ret = (uint64_t) 0;
				if( pR ) {
					*pR = N;
				}
				return ret;
			}

			if( N.size() > BARRETT_THRESHOLD ) {
				BigUInt barrettCheck = D << 1;
				if( N < barrettCheck ) {
					return BigUInt::BarrettReduction(N, D, pR);
				}
			}

			return BigUInt::DACDivide(N, D, pR);

		}

		static BigUInt
		ModExp(BigUInt &inBase, BigUInt &inExp, BigUInt &mod) {
			
			BigUInt ret = 1;
			BigUInt base = inBase % mod;
			BigUInt exp = inExp;

			while( exp > 0 ) {
				BigUInt check1 = exp % 2;
				if( check1 == 1 ){
					ret = ret * base;
					ret = ret % mod;
				}
				exp = exp >> 1; // exp / 2
				base = base * base;
				base = base % mod;
			}

			return ret;
		}

		string
		toString() const {

			BigUInt thisCopy = *this;

			std::stringstream ss;
			string out;

			int maxDigit = 18;
			BigUInt d = 0xDE0B6B3A7640000;

			BigUInt r;
			BigUInt q;
			while( thisCopy != 0 ) {

				q  = DivideWithRemainder( thisCopy, d, &r );

				ss << r.toUint64();
				
				string val = ss.str();
				if( q != 0 ) {
					while( val.length() != maxDigit ) {
						val = "0" + val;
					}
				} 

				out = val + out;
				ss.str(std::string());

				thisCopy = q;
			}

			return out;

		}

		string
		toHexString() const {
			std::stringstream ss;
			auto i = value.rbegin();
			auto end = value.rend();
			while( i != end ) {
				ss << std::hex << *i;
				i++;
			}
			return ss.str();

		}

		BigUInt&
		operator+=( BigUInt &b ) {
			this->value.reserve(this->value.size() + b.value.size() + 1);
			uint64_t size = Pad(*this, b);
			uint64_t carry = 0;
			uint64_t i = 0;
			uint64_t result  = 0;
			uint64_t av = 0;
			uint64_t bv = 0;

			//Going through backwards, so prepend
			for (; i < size; i++) {
				av = this->value[i];
				bv = b.value[i];
				result = av + bv + carry;
				value[i] = result % MAX_LIMIT;
				carry = result / MAX_LIMIT;
			}

			while ( carry > 0 ) {
				prepend( carry % MAX_LIMIT );
				carry = carry / MAX_LIMIT;
			}

			BigUInt::Unpad(*this, b);
			return *this;
		}

	private:
		//Stores value in reverse
		vector<uint32_t> value;

		void
		initFromString(string in) {
			string process = in;
			BigUInt pow(1);
			BigUInt result((uint64_t)0);
			uint8_t e;

			if( process.substr(0,2) == "0x" ) {
				e = 0x10;
				process = process.substr(2);
				std::transform( process.begin(), process.end(), process.begin(), ::toupper );

				for( auto &hexIn : process ) {
					if( hexIn >= '0' && hexIn <= '9' ) {
						hexIn -= '0';
					} else if( hexIn >= 'A' && hexIn <= 'F' ) {
						hexIn -= 55;							
					} else {
						std::cerr << "Error in string - invalid character: " << hexIn << std::endl;
					}
				}
			} else {
				e = 10;
	
				for( auto &numIn : process ) {
					if( numIn >= '0' && numIn <= '9' ) {
						numIn -= '0';
					} else {
						std::cerr << "Error in string - invalid character: " << numIn << std::endl;
					}
				}
			}

			std::reverse( process.begin(), process.end() );
			for( auto c : process ) {
				result = pow * c;
				*this += result;
				pow = pow * e;
			}

		}

		void prepend( uint32_t a ) {
			value.push_back( a );
		}

		void append( uint32_t a ) {
			value.insert( value.begin(), a );
		}

		static BigUInt
		LeftShift( BigUInt &a ) {
			BigUInt ret = a;
			
			bool addEntry = false;
			auto i = ret.value.rbegin();
			auto end = ret.value.rend();
			auto next = ret.value.rbegin();
			next++;
			uint32_t carry = 0;
		
			//Overflow into new entry
			if( (*i) & 0x80000000 ) {
				addEntry = true;	
			}

			while( i != end ) {

				if( next != end && (*next) & 0x80000000 ){
					carry = 1;	
				} else {
					carry = 0;
				}

				*i = (*i) << 1;
				*i = *i + carry;

				i++;
				next++;
			}

			if( addEntry ) {
				ret.prepend(0x1);
			}

			return ret;
		}
	
		static BigUInt
		RightShift( BigUInt &a ) {
			BigUInt ret = a;

			auto i = ret.value.begin();
			auto end = ret.value.end();
			auto next = ret.value.begin();
			next++;
			uint32_t carry = 0;

			while( i != end ) {

				if( next!= end && (*next) & 0x1 ){
					carry = 0x80000000;	
				} else {
					carry = 0;
				}

				*i = (*i) >> 1;
				*i = *i + carry;

				i++;
				next++;
			}
			
			return ret;
		}
		static BigUInt
		Mul_u32int( BigUInt &a, uint32_t b ) {
			
			BigUInt ret;

			if( b == 2 ) {
				ret = a << 1;
				return ret;
			}

			uint64_t bv = b;
			uint64_t carry = 0;	
			uint64_t value = 0;

			//Starting from LSB, so prepend values
			for( uint64_t i : a.value ) {
				value = i * bv + carry;
				carry = value >> 32;
				ret.prepend(value & UINT64_LOW);
			}

			if( carry ) {
				ret.prepend(carry);
			}
			return ret;

		}
		
		static uint64_t	
		Pad( BigUInt &a, BigUInt &b ) {
			if ( a.value.size() > b.value.size() ) {
				int diff = (a.value.size() - b.value.size());
				for ( int i = 0; i < diff; ++i ) {
					b.prepend(0);
				}
			} else {
				int diff = (b.value.size() - a.value.size());
				for ( int i = 0; i < diff; ++i ) {
					a.prepend(0);
				}
			}
			return a.value.size();
		}

		static void
		Unpad( BigUInt &a, BigUInt &b ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
			
			while(b.value.back() == 0 && b.value.size() != 1) {
				b.value.pop_back();
			}
		}
		
		static void
		Unpad( BigUInt &a ) {
			while(a.value.back() == 0 && a.value.size() != 1) {
				a.value.pop_back();
			}
		}

		bool
		isEven() {
			return value.size() > 0 ? !(0x1 & value[0]) : false;
		}

		static BigUInt
		ExpBySquaring( BigUInt &x, BigUInt &n ) {

			if( n == 0 ) {
				return 1;
			} else if( n == 1 ) {
				return x;
			} else if ( n.isEven() ) {
				BigUInt newX = x * x;
				BigUInt newN = n / 2;
				return ExpBySquaring( newX, newN );
			} else {
				BigUInt newX = x * x;
				BigUInt newN = n - 1;
				newN = newN / 2;
				BigUInt ret = ExpBySquaring( newX, newN );
				ret = ret * x;
				return ret;
			}

		}

		static BigUInt
		BarrettReduction ( BigUInt &N, BigUInt &D, BigUInt *pR = nullptr) {

			//Calculate k
			uint64_t back = D.value.back();
			uint64_t k = 0;
			while( back >>= 1 ) {
				++k;
			}

			k += (D.size() > 1 ? D.size() - 1 : 0) * 32;
			
			//Calculate m
			BigUInt mBase = 4;
			BigUInt FourPowK = mBase ^ k;
			BigUInt m = FourPowK / D;
			
			//Q
			BigUInt q = m * N;
			q = q / FourPowK;
			
			if( pR ) {
				BigUInt r = q * D;
				r = N - r;
			
				*pR = r < D ? r : r - D;
			}
			
			return q;	
			
		} 
		 


		static BigUInt
		DACDivide( BigUInt &N, BigUInt &D, BigUInt *pRemain = NULL ) {
			BigUInt ret((uint64_t)0);
	
			uint64_t nSize = N.size();
			uint64_t dSize = D.size();
			uint64_t range = nSize - dSize;
		
			BigUInt maxCalc = range ? (range + 1) * 32: 0;
			BigUInt max = 0xFFFFFFFF;
			max = max << maxCalc;

			BigUInt minCalc = range ? (range - 1) * 32 : 0;
			BigUInt min = 1;
			min = min << minCalc;
			
			BigUInt half = max - min;
			half = half >> 1;
			half += min;

			while(true) {
				BigUInt result = half * D;
				
				if( result == N ) {
					if( pRemain ) {
						*pRemain = ret;
					}
					return half;
				}

				if( N < result ) {
					max = half;
				} else {
					min = half;
				}

				half = max - min;

				//TODO this is kind of ugly
				//maybe cleaner solution when half < 5
				if( half < 5 ) {
					max = max + 1;
					while( min != max ) {
						result = min * D;
						if( N == result ) {
							if( pRemain ) {
								*pRemain = (uint64_t)0;
							}
							return min;
						} else if( N < result ) {
							BigUInt ret = min - 1;
							if( pRemain ) {
								result = ret * D;
								*pRemain = N - result;
							}
							return ret;
						}
						min = min + 1;

					}
				}

				half = half >> 1; // half / 2
				half += min;
			}

		}

		static BigUInt
		Karatsuba( BigUInt &a, BigUInt &b ) {
			if( a.size() == 0 || b.size() == 0 ) {
				std::cerr << "Error: Karatsuba - a value has no contents." << std::endl;
			}
			
			if( a.size() == 1 || b.size() == 1 ) {
				return a * b;
			}
			
			BigUInt ret;

			uint64_t m = Pad(a, b);
			uint64_t m2 = m/2;

			ret.value.reserve(m*2);
			
			BigUInt  low1(a.value.begin()     , a.value.begin() + m2);
			BigUInt high1(a.value.begin() + m2, a.value.end());
			
			BigUInt  low2(b.value.begin()     , b.value.begin() + m2);
			BigUInt high2(b.value.begin() + m2, b.value.end());

			BigUInt z0 = Karatsuba(low1, low2);
		
			BigUInt lowhigh1 = low1 + high1;
			BigUInt lowhigh2 = low2 + high2;
			BigUInt z1 = Karatsuba(lowhigh1, lowhigh2);
			
				
			BigUInt z2 = Karatsuba(high1, high2);
			
			BigUInt zdiff = z1 - z2;
			zdiff = zdiff - z0;

			for (int i = 0; i < m2 * 2; i++) {
				z2.append(0);
			}

			for (int i = 0; i < m2; i++) {
				zdiff.append(0);
			}
		
			ret = z2 + zdiff;
			ret = ret + z0;

			Unpad(a, b);
			Unpad(ret);

			return ret;

		}

	friend bool operator==( BigUInt &a, BigUInt &b );
	friend bool operator==( BigUInt &a, uint32_t b );
	friend bool operator!=( BigUInt &a, BigUInt &b );
	friend bool operator!=( BigUInt &a, uint32_t b );
	friend bool operator< (BigUInt &a, BigUInt &b);
	friend bool operator< (BigUInt &a, uint32_t b);
	friend bool operator> (BigUInt &a, uint32_t b);
	friend std::ostream& operator <<(std::ostream& stream, const BigUInt& a);
	friend BigUInt operator <<( BigUInt &a, BigUInt &b );
	friend BigUInt operator <<( BigUInt &a, uint64_t b );
	friend BigUInt operator >>( BigUInt &a, uint64_t b );
	friend BigUInt operator+( BigUInt &a, BigUInt &b );
	friend BigUInt operator+( BigUInt &a, uint32_t b );
	friend BigUInt operator-( BigUInt &a, BigUInt &b );
	friend BigUInt operator-( BigUInt &a, uint32_t b );
	friend BigUInt operator*( BigUInt &a, BigUInt &b );
	friend BigUInt operator*( BigUInt &a, uint32_t b );
	friend BigUInt operator^( BigUInt &a, BigUInt &b );
	friend BigUInt operator^( BigUInt &a, uint32_t b );
	friend BigUInt operator/( BigUInt &a, BigUInt &b );
	friend BigUInt operator/( BigUInt &a, uint32_t b );
	friend BigUInt operator%( BigUInt &a, BigUInt &b );
	friend BigUInt operator%( BigUInt &a, uint32_t b );
};

BigUInt
operator>>( BigUInt &a, uint64_t b ) {
	BigUInt ret = a;
	for( uint64_t i = 0; i < b; ++i) {
		ret = BigUInt::RightShift(ret);
	}
	BigUInt::Unpad(ret);
	return ret;
}

BigUInt
operator<<( BigUInt &a, BigUInt &b ) {
	BigUInt ret = a;
	for( BigUInt count = b ; count > 0; count = count - 1) {
		ret = BigUInt::LeftShift(ret);
	}
	return ret;

}


BigUInt
operator<<( BigUInt &a, uint64_t bv ) {
	BigUInt b(bv);
	return operator<<(a, b);

}

BigUInt
operator%( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	BigUInt::DivideWithRemainder(a, b, &ret);
	return ret;
}

BigUInt
operator%( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator%(a, b);
}

BigUInt
operator/( BigUInt &N, BigUInt &D ) {
	return BigUInt::DivideWithRemainder(N, D, NULL);
}

BigUInt
operator/( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator/(a, b);
}

BigUInt
operator^( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator^(a, b);
}

BigUInt
operator^( BigUInt &a, BigUInt &b ) {
	return BigUInt::ExpBySquaring(a, b);
}

bool
operator> (BigUInt &a, uint32_t b) {
	if( a.value.size() == 1 ) {
		return a.value[0] > b;
	}

	if( a.value.size() > 1 ) {
		return true;
	}

	if( a.value.size() == 0 ) {
		std::cerr << "Error: BigUInt::operator> - left hand contains no value" << std::endl;
	}

	return false;
}

bool
operator< (BigUInt &a, BigUInt &b) {
	if( a.value.size() != b.value.size() ) {
		return a.value.size() < b.value.size();
	} else {
		for( int i = a.value.size()-1; i >= 0; --i ) {
			if( a.value[i] != b.value[i] ) {
				return a.value[i] < b.value[i];
			}
		}
	}
	return false;
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
	uint64_t size = BigUInt::Pad(a, b);
	ret.value.reserve(size);
	uint64_t i = 0;
	bool borrow = false;
	int64_t result  = 0;

	//Going through backwards, so prepend
	for (; i < size; i++) {
		uint64_t av = a.value[i];
		uint64_t bv = b.value[i];
		result = av - bv;
		if( borrow ) {
			result -= 1;
			borrow = false;
		}

		if( result <  0 ) {
			result = MAX_LIMIT + result;
			borrow = true;
		}

		ret.prepend( result % MAX_LIMIT );
	}

	if( borrow ) {
		uint64_t v = ret.value[i];
		ret.prepend( v - 1 );
	}

	BigUInt::Unpad(a, b);
	BigUInt::Unpad(ret);
	return ret;
}

BigUInt
operator+( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator+(a, b);
}

BigUInt
operator+( BigUInt &a, BigUInt &b ) {
	BigUInt ret;
	ret.value.reserve(a.value.size() + b.value.size() + 1);
	uint64_t size = BigUInt::Pad(a, b);
	uint64_t carry = 0;
	uint64_t i = 0;
	uint64_t result  = 0;
	uint64_t av = 0;
	uint64_t bv = 0;

	//Going through backwards, so prepend
	for (; i < size; i++) {
		av = a.value[i];
		bv = b.value[i];
		result = av + bv + carry;
		ret.prepend( result % MAX_LIMIT );
		carry = result / MAX_LIMIT;
	}

	if ( carry != 0 ) {
		ret.prepend( carry );
	}

	BigUInt::Unpad(a, b);
	return ret;
}

std::ostream&
operator <<(std::ostream& stream, const BigUInt& a) {
	string outs = a.toString();
	stream << outs;
	return stream;
}

BigUInt
operator*( BigUInt &a, BigUInt &b ) {
	if( (a.value.size() == 1 && a.value[0] == 0) || (b.value.size() == 1 && b.value[0] == 0) ) {
		return (uint64_t)0;			
	} else if (a.value.size() == 1 && b.value.size() == 1 ) {
		uint64_t av = a.value[0];
		uint64_t bv = b.value[0];
		return BigUInt(av * bv);
	} else if (  a.value.size() == 1 || b.value.size() == 1 ) {
		if ( a.value.size() == 1 ) {
			return BigUInt::Mul_u32int( b, a.value[0] );
		} else {
			return BigUInt::Mul_u32int( a, b.value[0] );
		}
	} else 	{
		return BigUInt::Karatsuba(a, b);
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
operator!=( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return !operator==(a, b);
}

bool
operator==( BigUInt &a, uint32_t bv ) {
	BigUInt b(bv);
	return operator==(a, b);
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
